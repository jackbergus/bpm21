package main;

import java.io.*;
import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;
import com.google.gson.Gson;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import control.H_ConstraintsPerspective;
import control.H_MenuPerspective;
import control.H_PlannerPerspective;
import control.H_TracesPerspective;
import uk.knobab.ElaborateOnAlignment;
import view.PlannerOrchestrator;

import static control.H_MenuPerspective.normaliseActivityLabelForPlanner;

public class Main {

    public static class AlignmentOverallResults {
        public double planner_time_ms;
        public int ntraces;
        public long conversion_time_ms;
        public boolean isLTLf;
        public boolean isXesFile;

        List<ElaborateOnAlignment.AlignmentOutcome> lsx;

        public AlignmentOverallResults() {
        }
    }

    public static class QueryForAlignment {
        Map<String, H_ConstraintsPerspective.CostMap> cost_map;
        public List<List<String>> atomised_trace_if_not_xes_path = new ArrayList<>();
        public String ltlfOrDFA = "", ifXesItsName = null;
        public boolean isLTLf;
        public boolean isXesFile;
        public List<List<String>> eq_classes; // representation as a list of pairs
        public String eq_classes_file;
    }

    public static void init() {
        //Force the executable files of Fast-downward and LPG to be executable
        new File("translate_script").setExecutable(true);
        new File("preprocess_script").setExecutable(true);
        new File("planner_subopt_script").setExecutable(true);
        new File("planner_opt_script").setExecutable(true);
        new File("lpg_script").setExecutable(true);
        new File("fast-downward/src/translate/translate.py").setExecutable(true);
        new File("fast-downward/src/preprocess/preprocess").setExecutable(true);
        new File("fast-downward/src/search/downward").setExecutable(true);
        new File("fast-downward/src/search/unitcost").setExecutable(true);
        new File("fast-downward/src/search/downward-release").setExecutable(true);
        new File("LPG/lpg").setExecutable(true);
        new File("run_FD").setExecutable(true);
        new File("run_FD_all").setExecutable(true);
        new File("run_SYMBA").setExecutable(true);
        new File("run_SYMBA_all").setExecutable(true);
        new File("checkNumberOfTraces").setExecutable(true);
    }

    public static void main(String[] args) throws IOException {
        Gson gson = new Gson();
        init();
        int port = 6666;
        String address = "localhost";
        if (args.length >= 1) {
            port = Integer.parseInt(args[0]);
        }
        if (args.length >= 2) {
            address = args[1];
        }
        ThreadPoolExecutor threadPoolExecutor = (ThreadPoolExecutor) Executors.newFixedThreadPool(1);
        HttpServer server = HttpServer.create(new InetSocketAddress(address, port), 0);
        server.createContext("/stop", (e) -> System.exit(0));
        server.createContext("/savvy", (HttpExchange exchange) -> {
            var d = new PlannerOrchestrator();
            String json = new String(exchange.getRequestBody().readAllBytes(), StandardCharsets.UTF_8);
            QueryForAlignment query = gson.fromJson(json, QueryForAlignment.class);
            /// Setting up the traces to be analysed
            AlignmentOverallResults result = new AlignmentOverallResults();
            Multimap<String, String> act_to_atoms = HashMultimap.create();
            if ((query.eq_classes_file != null) && (!query.eq_classes_file.isEmpty())) {
                try (BufferedReader br = new BufferedReader(new FileReader(query.eq_classes_file))) {
                    String line;
                    if (query.eq_classes == null)
                        query.eq_classes = new ArrayList<>();
                    else
                        query.eq_classes.clear();
                    while ((line = br.readLine()) != null) {
                        String[] vars = line.split(" ");

                        // Just for debugging purposes:
                        List<String> lst = new ArrayList<>();
                        lst.add(vars[0].replace("\n", ""));
                        lst.add(vars[1].replace("\n", ""));
                        query.eq_classes.add(lst);
                        query.eq_classes_file = null;
                        /////////////////////////////////

                        act_to_atoms.put(normaliseActivityLabelForPlanner(vars[0].replace("\n", "")),
                                normaliseActivityLabelForPlanner(vars[1].replace("\n", "")));
                        // process the line.
                    }
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            } else {
                if (query.eq_classes.isEmpty()) {
                    for (var trace : query.atomised_trace_if_not_xes_path)
                        for (var event : trace)
                            act_to_atoms.put(normaliseActivityLabelForPlanner(event), normaliseActivityLabelForPlanner(event));
                } else {
                    for (var eq1 : query.eq_classes) {
                        act_to_atoms.put(normaliseActivityLabelForPlanner(eq1.get(0)), normaliseActivityLabelForPlanner(eq1.get(1)));
                    }
                }
            }
            result.isXesFile = query.isXesFile;
            result.isLTLf = query.isLTLf;
            // How to load the stuff altogether!
            // Mimicking a human clicker!
            Date startDate = new Date();
            if (query.isXesFile) {
                query.isXesFile = false;
                query.atomised_trace_if_not_xes_path = H_MenuPerspective.deserializeAtomisedXESFile(new File(query.ifXesItsName));
                query.ifXesItsName = null;
            }
            H_MenuPerspective.loadXESFile(query.atomised_trace_if_not_xes_path);
            H_TracesPerspective.afterLoadingTraces(d.getTracePerspective());
            H_MenuPerspective.setDataAwareMap(act_to_atoms);
            if (query.isLTLf) {
                H_MenuPerspective.addLTLFFormula(query.ltlfOrDFA); // A string, not the path to it!
            } else {
                if (query.ltlfOrDFA.startsWith("file://")) {
                    query.ltlfOrDFA = Files.readString(new File(query.ltlfOrDFA.replace("file://", "")).toPath());
                }
                H_MenuPerspective.addDotAutomaton(query.ltlfOrDFA);
                // The path to the automaton!, might be changed to something else later on...
            }
            H_ConstraintsPerspective.setUpModel(d.getConstraintPerspective(), normaliseQueryCostMap(query.cost_map));
            H_PlannerPerspective.reduceToPlanningProblem(d.getPlannerPerspective());
            Date endDate = new Date();
            result.conversion_time_ms = endDate.getTime() - startDate.getTime();
            String[] callCmd = {"/bin/bash", "-c", "./run_SYMBA_ML_all"};
            var map = execCommand(callCmd);
            H_PlannerPerspective.dumpOutputs();
            result.lsx = new ArrayList<>();
            if (map.get(0).equals("0")) {
                System.out.println("exit code:\n" + map.get(0));
                String[] results = map.get(1).split("\n");
                String benchmark = results[results.length - 1].replace("the duration is: ", "");
                String[] ms_to_n_traces = benchmark.split(" ms\\. for ");
                result.planner_time_ms = Double.parseDouble(ms_to_n_traces[0]);
                result.ntraces = Integer.parseInt(ms_to_n_traces[1].replace(" traces", ""));
                for (int i = 0; i < result.ntraces; i++) {
                    result.lsx.add(ElaborateOnAlignment.parseFile(new File("seq-opt-symba-2", results[i * 3])));
                }
            }
            String json_result = gson.toJson(result);
            byte[] response = json_result.getBytes();
            exchange.sendResponseHeaders(200, response.length);
            OutputStream os = exchange.getResponseBody();
            os.write(response);
            os.close();
        });
        server.setExecutor(threadPoolExecutor);
        server.start();
    }
    private static Map<String, H_ConstraintsPerspective.CostMap> normaliseQueryCostMap(Map<String, H_ConstraintsPerspective.CostMap> cost_map) {
        Map<String, H_ConstraintsPerspective.CostMap> result = new HashMap<>();
        for (var cp : cost_map.entrySet()) {
            result.put(normaliseActivityLabelForPlanner(cp.getKey()), normaliseCostMap(cp.getValue()));
        }
        return result;
    }
    private static H_ConstraintsPerspective.CostMap normaliseCostMap(H_ConstraintsPerspective.CostMap value) {
        return new H_ConstraintsPerspective.CostMap(normaliseActivityLabelForPlanner(value.action), value.adding_cost, value.removal_cost);
    }

    public static Map<Integer, String> execCommand(String... str) {
        Map<Integer, String> map = new HashMap<>();
        ProcessBuilder pb = new ProcessBuilder(str);
        pb.redirectErrorStream(true);
        Process process = null;
        try {
            process = pb.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
        BufferedReader reader = null;
        if (process != null) {
            reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
        }

        String line;
        StringBuilder stringBuilder = new StringBuilder();
        try {
            if (reader != null) {
                while ((line = reader.readLine()) != null) {
                    stringBuilder.append(line).append("\n");
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        try {
            if (process != null) {
                process.waitFor();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (process != null) {
            map.put(0, String.valueOf(process.exitValue()));
        }

        try {
            map.put(1, stringBuilder.toString());
        } catch (StringIndexOutOfBoundsException e) {
            if (stringBuilder.toString().length() == 0) {
                return map;
            }
        }
        return map;
    }

}
