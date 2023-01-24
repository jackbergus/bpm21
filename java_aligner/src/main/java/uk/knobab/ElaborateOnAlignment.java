package uk.knobab;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class ElaborateOnAlignment {
    static Pattern synch = Pattern.compile("\\(sync\\-([^\\-]+)-[^\\)]+\\)");
    static Pattern add = Pattern.compile("\\(add\\-([^\\-]+)-[^\\)]+\\)");
    static Pattern del = Pattern.compile("\\(del\\-([^\\-]+)-[^\\)]+\\)");

    static Pattern repl = Pattern.compile("\\(add\\-repl\\-([^\\-]+)-with-([^\\-]+)-[^\\)]+\\)");

    /**
     * Tests the match with the synch
     * @param str	Pattern to be tested
     * @return		If this is a synch, then the sync corresponding to the given event is given. If this did not match, an empty string is returned instead
     */
    public static String matchSynch(String str) {
        var out = synch.matcher(str);
        if (out.matches()) {
            return out.group(1);
        } else return "";
    }
    /**
     * Tests the match with the addition
     * @param str	Pattern to be tested
     * @return		If this is an addition, then the addition corresponding to the given event is given. If this did not match, an empty string is returned instead
     */
    public static String matchAdd(String str) {
        var out = add.matcher(str);
        if (out.matches()) {
            String outcome = out.group(1);
            if (outcome.equals("repl") && str.chars().filter(ch -> ch == '-').count() == 2)
                return outcome;
        }
        return "";
    }
    /**
     * Tests the match with the deletion
     * @param str	Pattern to be tested
     * @return		If this is a deletion, then the deletion corresponding to the given event is given. If this did not match, an empty string is returned instead
     */
    public static String matchDel(String str) {
        var out = del.matcher(str);
        if (out.matches()) {
            String outcome = out.group(1);
            if (outcome.equals("repl") && str.chars().filter(ch -> ch == '-').count() == 3)
                return outcome;
        }
        return "";
    }

    public static List<String> matchRepl(String str) {
        List<String> ls = new ArrayList<>(2);
        var out = repl.matcher(str);
        if (out.matches()) {
            ls.add(out.group(1));
            ls.add(out.group(2));
        }
        return ls;
    }

    public static enum AlignmentCases {
        ALIGN_SYNC,
        ALIGN_DEL,
        ALIGN_ADD,
        ALIGN_REPLACE_WITH;
    }

    public static class AlignmentStrategy  {
        public AlignmentCases cases;
        public String action;
        public String with;

        public AlignmentStrategy(AlignmentCases cases, String action, String with) {
            this.cases = cases;
            this.action = action;
            this.with = with;
        }
    }

    public static class AlignmentOutcome {
        public List<AlignmentStrategy> alignments;
        public long current_trace_id = -1;
        public List<String> original_atomised_trace = Collections.EMPTY_LIST;
        public long trace_length = -1;
        public long aligned_trace_length = -1;
        public double alignment_cost = 0;
        public long number_of_alignments = 0;
        public double fitness = 0.0;

        public AlignmentOutcome() {
        }
    }

    public static AlignmentOutcome parseFile(File f) throws IOException {
        AlignmentOutcome result = new AlignmentOutcome();
        result.alignments = new ArrayList<>();
        BufferedReader bf = new BufferedReader(new FileReader(f));
        // read entire line as string
        String line = bf.readLine();
        boolean collectAlignmentInformation = true;
        int count = 0;
        // checking for end of file
        while (line != null) {
            if (!line.isEmpty()) {
                if (collectAlignmentInformation) {
                    String candidate;
                    candidate = matchSynch(line);
                    if (!candidate.isEmpty()) {
                        result.alignments.add(new AlignmentStrategy(AlignmentCases.ALIGN_SYNC, candidate, ""));
                        line = bf.readLine();
                        continue;
                    }
                    List<String> candidateLS = matchRepl(line);
                    if (!candidateLS.isEmpty()) {
                        result.alignments.add(new AlignmentStrategy(AlignmentCases.ALIGN_REPLACE_WITH, candidateLS.get(0), candidateLS.get(1)));
                        line = bf.readLine();
                        continue;
                    }
                    candidate = matchAdd(line);
                    if (!candidate.isEmpty()) {
                        result.alignments.add(new AlignmentStrategy(AlignmentCases.ALIGN_ADD, candidate, ""));
                        line = bf.readLine();
                        continue;
                    }
                    candidate = matchDel(line);
                    if (!candidate.isEmpty()) {
                        result.alignments.add(new AlignmentStrategy(AlignmentCases.ALIGN_DEL, candidate, ""));
                        line = bf.readLine();
                        continue;
                    }
                } else {
                    if (count == 0) {
                        result.current_trace_id = Long.valueOf(line.replace(">>>> TRACE ID: Trace#", "").split(" ")[0]);
                    } else if (count == 1) {
                        result.original_atomised_trace = Arrays.stream(line.replace(">>>> TRACE CONTENT: [", "").replace("]", "").split(",")).map(String::strip).collect(Collectors.toList());
                    } else if (count == 2) {
                        result.trace_length = Long.valueOf(line.replace(">>>> ORIGINAL TRACE LENGTH: ", ""));
                    } else if (count == 3) {
                        result.aligned_trace_length = Double.valueOf(line.replace(">>>> ALIGNED TRACE LENGTH: ", "")).longValue();
                    } else if (count == 4) {
                        result.alignment_cost = Double.valueOf(line.replace(">>>> COST OF ALIGNMENT: ", "")).longValue();
                    } else if (count == 5) {
                        result.number_of_alignments = Double.valueOf(line.replace(">>>> NUMBER OF ALIGNMENTS: ", "")).longValue();
                    } else if (count == 6) {
                        result.fitness = Double.valueOf(line.replace(">>>> FITNESS: ", ""));
                    }
                    count++;
                }
            } else {
                collectAlignmentInformation = false;
            }
            line = bf.readLine();
        }

        // closing bufferreader object
        bf.close();
        return result;
    }


}
