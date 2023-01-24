package control;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;
import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.processmining.plugins.declare.visualizing.AssignmentModel;
import org.processmining.plugins.declare.visualizing.AssignmentViewBroker;
import org.processmining.plugins.declare.visualizing.ConstraintDefinition;
import org.processmining.plugins.declare.visualizing.Parameter;
import org.processmining.plugins.declare.visualizing.XMLBrokerFactory;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;

import main.Constants;
import main.Trace;
import main.XLogReader;

import view.MenuPerspective;

public class H_MenuPerspective {
	
	public MenuPerspective _view = null;
	
	public H_MenuPerspective (MenuPerspective i_view){
		_view = i_view;
		installListeners();
	}
//
//	public  static <T extends Iterable<String>> void loadAtomisedTrace(Iterable<T> log) {
//		////////////////////////////////////////////////////////////////
//		// RESET the AlphabetPerspective panel
//		clearAlphabetPerspective();
//
//		// RESET the TracesPerspective panel
//		clearTracePerspective();
//
//		// RESET the ConstraintsPerspective panel
//		Constants.getConstraintsPerspective().resetComponent();
//
//		Constants.getAlphabetPerspective().setComponentEnabled(false);
//		Constants.getTracesPerspective().setComponentEnabled(true);
//		Constants.getConstraintsPerspective().setComponentEnabled(false);
//		////////////////////////////////////////////////////////////////
//		int trace_int_id = 0;
//
//		// Vector used to record the complete alphabet of activities used in the log traces.
//		Vector<String> loaded_alphabet_vector = new Vector<>();
//
//		// Vector used to record the activities (in their original order) of a specific trace of the log.
//		Vector<String> loaded_trace_activities_vector;
//		for (T trace : log) {
//			trace_int_id++;
//			String traceName = "trace" + trace_int_id;
//			Trace t = new Trace("Trace#" + trace_int_id, traceName);
//			//UPDATE the JComboBox of the GUI with the loaded traces
//			Constants.getTracesPerspective().getTracesComboBox().addItem(t.getTraceID());
//			//////////////////////////////////////////////
//
//			//XAttributeMap caseAttributes = trace.getAttributes();
//			loaded_trace_activities_vector = new Vector<>();
//			for (String event : trace) {
//				String activityName = event.toLowerCase();
//				if (activityName.contains(" "))
//					activityName = activityName.replaceAll(" ", "");
//				if (activityName.contains("/"))
//					activityName = activityName.replaceAll("\\/", "");
//				if (activityName.contains("("))
//					activityName = activityName.replaceAll("\\(", "");
//				if (activityName.contains(")"))
//					activityName = activityName.replaceAll("\\)", "");
//				if (activityName.contains("<"))
//					activityName = activityName.replaceAll("\\<", "");
//				if (activityName.contains(">"))
//					activityName = activityName.replaceAll("\\>", "");
//
//				if (activityName.contains("."))
//					activityName = activityName.replaceAll("\\.", "");
//
//				if (activityName.contains(","))
//					activityName = activityName.replaceAll("\\,", "_");
//
//				if (activityName.contains("+"))
//					activityName = activityName.replaceAll("\\+", "_");
//
//				if (activityName.contains("-"))
//					activityName = activityName.replaceAll("\\-", "_");
//				loaded_trace_activities_vector.addElement(activityName);
//				if (!loaded_alphabet_vector.contains(activityName))
//					loaded_alphabet_vector.addElement(activityName);
//			}
//
//			//
//			// Update the single trace of the log
//			//
//			String trace_content = "";
//
//			for(int j=0;j<loaded_trace_activities_vector.size();j++) {
//				String string = loaded_trace_activities_vector.elementAt(j);
//
//				if(!t.getTraceAlphabet_vector().contains(string)) {
//					t.getTraceAlphabet_vector().addElement(string);
//				}
//
//				// Hashtable used to set the number of activity instances of a trace
//				if(t.getNumberOfActivityInstances_Hashtable().containsKey(string)) {
//					int number_of_instances = t.getNumberOfActivityInstances_Hashtable().get(string);
//					number_of_instances = number_of_instances + 1;
//					t.getNumberOfActivityInstances_Hashtable().put(string, number_of_instances);
//				}
//				else {
//					t.getNumberOfActivityInstances_Hashtable().put(string, 1);
//				}
//
//				for(int p=0;p<loaded_trace_activities_vector.size();p++) {
//
//					String string_key = string + p;
//
//					if(!t.getAssociationsToActivityInstances_Hashtable().containsKey(string_key)) {
//						t.getAssociationsToActivityInstances_Hashtable().put(string_key,string);
//						t.getTraceContentWithActivitiesInstances_vector().addElement(string_key);
//						t.getOriginalTraceContent_vector().addElement(string);
//						trace_content += string + ",";
//						break;
//					}
//				}
//			}
//			t.setOriginalTraceContentString(trace_content.substring(0, trace_content.length()-1));
//			Constants.getAllTraces_vector().addElement(t);
//		}
//		//Update the GUI component with the loaded LOG
//		Constants.setActivitiesRepository_vector(loaded_alphabet_vector);
//		for(int kix=0;kix<loaded_alphabet_vector.size();kix++){
//			Constants.getAlphabetPerspective().getAlphabetListModel().addElement(loaded_alphabet_vector.elementAt(kix));
//			Constants.getTracesPerspective().getAlphabetListModel().addElement(loaded_alphabet_vector.elementAt(kix));
//		}
//		Constants.getTracesPerspective().getTracesComboBox().setSelectedIndex(1);
//		Constants.getMenuPerspective().getImportMenu().setEnabled(false);
//		Constants.getMenuPerspective().getImportDeclareMenuItem().setEnabled(false);
//		/////////////////// ModelLearning ////////////////////
//		//////////////////////////////////////////////////////
//		Constants.getMenuPerspective().getImportModelLearningAutomatonMenuItem().setEnabled(false);
//		//////////////////////////////////////////////////////
//		//////////////////////////////////////////////////////
//
//		/////////////////// DataAware ////////////////////////
//		//////////////////////////////////////////////////////
//		Constants.getMenuPerspective().getimportDataAwareListMenuItem().setEnabled(false);
//		//////////////////////////////////////////////////////
//		//////////////////////////////////////////////////////
//	}

	private void installListeners() {
		
		_view.getExitMenuItem().addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent e) {

				Constants.getDesktop().dispose();
				
			}
		});
		
		_view.getNewMenuItem().addActionListener(e -> {
			String[] options = new String[] {"Yes", "No"};
			int response = JOptionPane.showOptionDialog(null, "Lose changes?", "Create new log traces and LTL/Declare constraints",JOptionPane.DEFAULT_OPTION, JOptionPane.PLAIN_MESSAGE,null, options, options[0]);
			if(response==0) {
				// RESET the AlphabetPerspective panel
				clearAlphabetPerspective();

				// RESET the TracesPerspective panel
				clearTracePerspective();

				// RESET the ConstraintsPerspective panel
				Constants.getConstraintsPerspective().resetComponent();

				Constants.getAlphabetPerspective().setComponentEnabled(true);
				Constants.getTracesPerspective().setComponentEnabled(false);
				Constants.getConstraintsPerspective().setComponentEnabled(false);
				Constants.getMenuPerspective().getImportMenu().setEnabled(false);
				Constants.getMenuPerspective().getImportDeclareMenuItem().setEnabled(false);
				 /////////////////// ModelLearning ////////////////////
				 //////////////////////////////////////////////////////
				 Constants.getMenuPerspective().getImportModelLearningAutomatonMenuItem().setEnabled(false);
				 //////////////////////////////////////////////////////
				 //////////////////////////////////////////////////////
				 /////////////////// DataAware ////////////////////////
				 //////////////////////////////////////////////////////
				 Constants.getMenuPerspective().getimportDataAwareListMenuItem().setEnabled(false);
				 //////////////////////////////////////////////////////
				 //////////////////////////////////////////////////////
			}
		});
		
		_view.getOpenMenuItem().addActionListener(e -> {
			 JFileChooser fileChooser = new JFileChooser();
			 FileNameExtensionFilter xmlfilter = new FileNameExtensionFilter("Log file (*.xes)", "xes");

				 fileChooser.setDialogTitle("Open log file");
				 fileChooser.setAcceptAllFileFilterUsed(false);
				 fileChooser.setFileFilter(xmlfilter);

				 String workingDirectoryName = System.getProperty("user.dir");
				 File workingDirectory = new File(workingDirectoryName + File.separator + "resources" + File.separator + "log files");
				 fileChooser.setCurrentDirectory(workingDirectory);


				int returnValue = fileChooser.showOpenDialog(Constants.getDesktop());
				if (returnValue == JFileChooser.APPROVE_OPTION) {
				  File selectedFile = fileChooser.getSelectedFile();
				 // System.out.println(selectedFile.getName());

				  try {
					  loadXESFile(deserializeAtomisedXESFile(selectedFile));
				  }
				  catch (Exception exception) {
					exception.printStackTrace();
				}
				}
		});
		
		
		_view.getImportDeclareMenuItem().addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JFileChooser fileChooser = new JFileChooser();
				 FileNameExtensionFilter xmlfilter = new FileNameExtensionFilter("Declare file (*.xml)", "xml");
				 	fileChooser.setDialogTitle("Import Declare Constraints");
				 	fileChooser.setAcceptAllFileFilterUsed(false);
				 	fileChooser.setFileFilter(xmlfilter);
				 	
				 	String workingDirectoryName = System.getProperty("user.dir");
				 	File workingDirectory = new File(workingDirectoryName + File.separator + "resources" + File.separator + "declarative models");
				 	fileChooser.setCurrentDirectory(workingDirectory);
				 				 
			        int returnValue = fileChooser.showOpenDialog(Constants.getDesktop());
			        
			        if (returnValue == JFileChooser.APPROVE_OPTION) {
				         
			        	String[] options = new String[] {"Yes", "No"};
		        	    int response = JOptionPane.showOptionDialog(null, "Lose the previously defined Declare constraints?", "Import Declare constraints from a XML file",JOptionPane.DEFAULT_OPTION, JOptionPane.PLAIN_MESSAGE, null, options, options[0]);

		        	    if(response==0) {
			        	
						// RESET the ConstraintsPerspective view
						Constants.getConstraintsPerspective().resetComponent();
								        	    	
			        	File selectedFile = fileChooser.getSelectedFile();
				         
					  	try {
					  			 
					  			 AssignmentViewBroker broker = XMLBrokerFactory.newAssignmentBroker(selectedFile.getAbsolutePath());
	
					  			 AssignmentModel assmod = broker.readAssignment();
					  			 
					  			 for(ConstraintDefinition cd : assmod.getConstraintDefinitions()){

					  			  boolean is_valid_constraint = true;  
					  			  Vector<String> activities_not_in_the_repo_vector = new Vector<String>();
					  			  
					  			  String constraint = cd.getName() + "(";	
					  			  
					  			  int index = 0;
					  		    			  		    	  
					  		      for(Parameter p : cd.getParameters()){
					  		    	  
					  		       if(cd.getBranches(p).iterator().hasNext()){

									   String activityName = normaliseActivityLabelForPlanner(cd.getBranches(p).iterator().next().toString());

									   if(!Constants.getActivitiesRepository_vector().contains(activityName)) {
											activities_not_in_the_repo_vector.addElement(activityName);
											is_valid_constraint = false;
										}
																				
					  		         cd.getBranches(p).iterator().next();
					  		         
					  		         constraint = constraint + activityName;
					  		         
					  		         if(index<cd.getParameters().size()-1)
					  		        	constraint = constraint + ",";
					  		         index++;
					  		       }
					  		      }
					  		      					  		      
					  		      constraint = constraint + ")";
					  		      
					  		    if(!is_valid_constraint) {
					  		    	
					  		    	int dialogResult = 0;
					  		    	
					  		    	if(activities_not_in_the_repo_vector.size()==1)
					  		    		dialogResult = JOptionPane.showConfirmDialog(null, "The constraint '" + constraint + "' refers to the activity '" + activities_not_in_the_repo_vector.elementAt(0) + "',\nwhich is not listed in the activities repository! Such a constraint can not be properly imported, unless the missing activity is not imported in the repository.\n\nDo you want to import the activity '" + activities_not_in_the_repo_vector.elementAt(0) + "' in the activities repository?", "ATTENTION!", JOptionPane.YES_NO_OPTION);
					  		    	else
					  		    		dialogResult = JOptionPane.showConfirmDialog(null, "The constraint '" + constraint + "' refers to the activities: \n" + activities_not_in_the_repo_vector + ", which are not listed in the activities repository!\nSuch a constraint can not be properly imported, unless the missing activities are not imported in the repository.\n\nDo you want to import the activities " + activities_not_in_the_repo_vector + " in the activities repository?", "ATTENTION!", JOptionPane.YES_NO_OPTION);
					  		    		
					  		    	if(dialogResult == JOptionPane.YES_OPTION){
					  		    		for(int h=0;h<activities_not_in_the_repo_vector.size();h++) {
					  		    			String specific_activity = activities_not_in_the_repo_vector.elementAt(h);
					  		    			
					  		    			Constants.getActivitiesRepository_vector().addElement(specific_activity);											
					  		    			Constants.getAlphabetPerspective().getAlphabetListModel().addElement(specific_activity);
											Constants.getTracesPerspective().getAlphabetListModel().addElement(specific_activity);
					  		    		}
										Constants.getConstraintsPerspective().getConstraintsListModel().addElement(constraint);	
					  		    	}
					  		    }
					  		    else if(!Constants.getConstraintsPerspective().getConstraintsListModel().contains(constraint))
		            				Constants.getConstraintsPerspective().getConstraintsListModel().addElement(constraint);					  		      
					  			 
					  	}
					  	}
					  		catch (Exception exception) {
								exception.printStackTrace();
					        }
				        }
				        }
				  }
			});
		
		/////////////////// ModelLearning ////////////////////
		//////////////////////////////////////////////////////
		_view.getImportModelLearningAutomatonMenuItem().addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent e) {
				
				JFileChooser fileChooser = new JFileChooser();
				 
				    FileNameExtensionFilter xmlfilter = new FileNameExtensionFilter("DOT file (*.dot)", "dot");

				 	fileChooser.setDialogTitle("Import Automaton");
				 	fileChooser.setAcceptAllFileFilterUsed(false);
				 	fileChooser.setFileFilter(xmlfilter);
				 	
				 	String workingDirectoryName = System.getProperty("user.dir");
				 	File workingDirectory = new File(workingDirectoryName + File.separator + "resources" + File.separator + "declarative models");
				 	fileChooser.setCurrentDirectory(workingDirectory);
				 				 
			        int returnValue = fileChooser.showOpenDialog(Constants.getDesktop());
			        
			        if (returnValue == JFileChooser.APPROVE_OPTION) {
				         
			        	String[] options = new String[] {"Yes", "No"};
		        	    int response = JOptionPane.showOptionDialog(null, "Lose the previously defined Declare constraints?", "Import Declare constraints from a XML file",JOptionPane.DEFAULT_OPTION, JOptionPane.PLAIN_MESSAGE, null, options, options[0]);

		        	    if(response==0) {
			        	
						// RESET the ConstraintsPerspective view

								        	    	
			        	File selectedFile = fileChooser.getSelectedFile();
						addDotAutomaton(selectedFile);
			        	
				        }
				        }
				  }
			});
		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////
		
		/////////////////// DataAware ////////////////////
		//////////////////////////////////////////////////////
		_view.getimportDataAwareListMenuItem().addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent e) {
				
				JFileChooser fileChooser = new JFileChooser();
				 
				 FileNameExtensionFilter xmlfilter = new FileNameExtensionFilter("Text file (*.txt)", "txt");

				 	fileChooser.setDialogTitle("Import Data Atoms List");
				 	fileChooser.setAcceptAllFileFilterUsed(false);
				 	fileChooser.setFileFilter(xmlfilter);
				 	
				 	String workingDirectoryName = System.getProperty("user.dir");
				 	File workingDirectory = new File(workingDirectoryName + File.separator + "resources" + File.separator + "declarative models");
				 	fileChooser.setCurrentDirectory(workingDirectory);
				 				 
			        int returnValue = fileChooser.showOpenDialog(Constants.getDesktop());
			        
			        if (returnValue == JFileChooser.APPROVE_OPTION) {
				         
			        	Multimap<String, String> new_data_aware_map = HashMultimap.create();
			        	
			        	File selectedFile = fileChooser.getSelectedFile();
				        			        	
			        	try {
							BufferedReader reader = new BufferedReader(new FileReader(selectedFile));
							String line = reader.readLine();
							while(line!=null) {
							    // System.out.println(line);							     
							     if(line!=null)  {
								     String[] split_act = line.split(" ");
								     new_data_aware_map.put(split_act[0], split_act[1]);
							     }
							     line = reader.readLine();
							}
																				
						} catch (IOException e1) {
							e1.printStackTrace();
						}
						setDataAwareMap(new_data_aware_map);
				     }
				  }
			});
		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////
	
		_view.getImportLTLFormulaMenuItem().addActionListener(e -> {
			JFileChooser fileChooser = new JFileChooser();

			 FileNameExtensionFilter xmlfilter = new FileNameExtensionFilter("Text file (*.txt)", "txt");

				 fileChooser.setDialogTitle("Import LTL Formula");
				 fileChooser.setAcceptAllFileFilterUsed(false);
				 fileChooser.setFileFilter(xmlfilter);

				 String workingDirectoryName = System.getProperty("user.dir");
				 File workingDirectory = new File(workingDirectoryName + File.separator + "resources" + File.separator + "declarative models");
				 fileChooser.setCurrentDirectory(workingDirectory);

				int returnValue = fileChooser.showOpenDialog(Constants.getDesktop());

				if (returnValue == JFileChooser.APPROVE_OPTION) {

					File selectedFile = fileChooser.getSelectedFile();
					String ltl_formula= new String();
					try {
						BufferedReader reader = new BufferedReader(new FileReader(selectedFile));
						String line = reader.readLine();
						while(line!=null) {
						   //  System.out.println(line);
							 if(line!=null)  {
								 ltl_formula+=line;
							 }
							 line = reader.readLine();
						}

					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					addLTLFFormula(ltl_formula);
				 }
			  });
	}

	public static List<List<String>> deserializeAtomisedXESFile(File selectedFile) {
		List<List<String>> serialisedTraces;
		XLog log = null;
		try {
			log = XLogReader.openLog(selectedFile.getAbsolutePath());
		} catch (Exception e) {
			e.printStackTrace();
			return Collections.EMPTY_LIST;
		}
		serialisedTraces = new ArrayList<>();
		for(XTrace trace:log){
			List<String> lsk = new ArrayList<>();
			for(XEvent event : trace) {
				lsk.add(XConceptExtension.instance().extractName(event));
			}
			serialisedTraces.add(lsk);
		}
		return serialisedTraces;
	}

	public static void loadXESFile(List<List<String>> deserialisedFile)  {
		////////////////////////////////////////////////////////////////
		// RESET the AlphabetPerspective panel
		clearAlphabetPerspective();

		// RESET the TracesPerspective panel
		clearTracePerspective();

		// RESET the ConstraintsPerspective panel
		Constants.getConstraintsPerspective().resetComponent();

		Constants.getAlphabetPerspective().setComponentEnabled(false);
		Constants.getTracesPerspective().setComponentEnabled(true);
		Constants.getConstraintsPerspective().setComponentEnabled(false);
		////////////////////////////////////////////////////////////////



		int trace_int_id = 0;

		// Vector used to record the complete alphabet of activities used in the log traces.
		Vector<String> loaded_alphabet_vector = new Vector<>();

		// Vector used to record the activities (in their original order) of a specific trace of the log.
		Vector<String> loaded_trace_activities_vector;

		for(List<String> trace:deserialisedFile){
			trace_int_id++;

//			String traceName = XConceptExtension.instance().extractName(trace);
			//System.out.println("Trace Name : " + traceName);
			String traceName = "trace" + trace_int_id;

			Trace t = new Trace("Trace#" + trace_int_id,traceName);

			//UPDATE the JComboBox of the GUI with the loaded traces
			Constants.getTracesPerspective().getTracesComboBox().addItem(t.getTraceID());
			//////////////////////////////////////////////

			//XAttributeMap caseAttributes = trace.getAttributes();
			loaded_trace_activities_vector = new Vector<>();

			for(String curr : trace){
//				String activityName = XConceptExtension.instance().extractName(event);
//				lsk.add(activityName);
				String activityName = normaliseActivityLabelForPlanner(curr);
				loaded_trace_activities_vector.addElement(activityName);

				if(!loaded_alphabet_vector.contains(activityName))
					loaded_alphabet_vector.addElement(activityName);
			}

			//
			// Update the single trace of the log
			//
			String trace_content = "";

			for(int j=0;j<loaded_trace_activities_vector.size();j++) {
				  String string = loaded_trace_activities_vector.elementAt(j);

				  if(!t.getTraceAlphabet_vector().contains(string)) {
					t.getTraceAlphabet_vector().addElement(string);
				}

				  // Hashtable used to set the number of activity instances of a trace
				if(t.getNumberOfActivityInstances_Hashtable().containsKey(string)) {
					  int number_of_instances = t.getNumberOfActivityInstances_Hashtable().get(string);
					  number_of_instances = number_of_instances + 1;
					  t.getNumberOfActivityInstances_Hashtable().put(string, number_of_instances);
				  }
				  else {
					  t.getNumberOfActivityInstances_Hashtable().put(string, 1);
				  }

				  for(int p=0;p<loaded_trace_activities_vector.size();p++) {

					  String string_key = string + p;

					  if(!t.getAssociationsToActivityInstances_Hashtable().containsKey(string_key)) {
						  t.getAssociationsToActivityInstances_Hashtable().put(string_key,string);
						  t.getTraceContentWithActivitiesInstances_vector().addElement(string_key);
						  t.getOriginalTraceContent_vector().addElement(string);
						  trace_content += string + ",";
						  break;
					  }
				  }
				}
			t.setOriginalTraceContentString(trace_content.substring(0, trace_content.length()-1));
			Constants.getAllTraces_vector().addElement(t);
			/////////////////////////////////////////////////////////////
		}

		//Update the GUI component with the loaded LOG
		Constants.setActivitiesRepository_vector(loaded_alphabet_vector);
		for(int kix=0;kix<loaded_alphabet_vector.size();kix++){
			Constants.getAlphabetPerspective().getAlphabetListModel().addElement(loaded_alphabet_vector.elementAt(kix));
			Constants.getTracesPerspective().getAlphabetListModel().addElement(loaded_alphabet_vector.elementAt(kix));
		}

		Constants.getTracesPerspective().getTracesComboBox().setSelectedIndex(1);

		Constants.getMenuPerspective().getImportMenu().setEnabled(false);
		Constants.getMenuPerspective().getImportDeclareMenuItem().setEnabled(false);
		/////////////////// ModelLearning ////////////////////
		//////////////////////////////////////////////////////
		Constants.getMenuPerspective().getImportModelLearningAutomatonMenuItem().setEnabled(false);
		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////

		/////////////////// DataAware ////////////////////////
		//////////////////////////////////////////////////////
		Constants.getMenuPerspective().getimportDataAwareListMenuItem().setEnabled(false);
		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////
	}

	public static String normaliseActivityLabelForPlanner(String curr) {
		String activityName = curr.toLowerCase();
		if(activityName.contains(" "))
			activityName = activityName.replaceAll(" ", "");
		if(activityName.contains("/"))
			activityName = activityName.replaceAll("\\/", "");
		if(activityName.contains("("))
			activityName = activityName.replaceAll("\\(", "");
		if(activityName.contains(")"))
			activityName = activityName.replaceAll("\\)", "");
		if(activityName.contains("<"))
			activityName = activityName.replaceAll("\\<", "");
		if(activityName.contains(">"))
			activityName = activityName.replaceAll("\\>", "");
		if(activityName.contains("."))
			activityName = activityName.replaceAll("\\.", "");
		if(activityName.contains(","))
			activityName = activityName.replaceAll("\\,", "_");
		if(activityName.contains("+"))
			activityName = activityName.replaceAll("\\+", "_");
		if(activityName.contains("-"))
			activityName = activityName.replaceAll("\\-", "_");
		return activityName;
	}

	public static void clearAlphabetPerspective() {
		Constants.getAlphabetPerspective().resetComponent();
		Constants.setActivitiesRepository_vector(new Vector<>());
	}

	public static void clearTracePerspective() {
		Constants.getTracesPerspective().resetComponent();
		Constants.setAllTraces_vector(new Vector<>());
	}

	public static void addDotAutomaton(File selectedFile) {
		Constants.getConstraintsPerspective().resetComponent();
		String model_learning_constraint = "DFA{file://" + selectedFile.getAbsolutePath() + "}";
		Constants.getConstraintsPerspective().getConstraintsListModel().addElement(model_learning_constraint);
	}

	public static void addDotAutomaton(String dotstring) {
		Constants.getConstraintsPerspective().resetComponent();
		String model_learning_constraint = "DFA{" + dotstring + "}";
		Constants.getConstraintsPerspective().getConstraintsListModel().addElement(model_learning_constraint);
	}

	public static void setDataAwareMap(Multimap<String, String> multimap) {
		Constants.setDataAware_map(multimap); // Import Data Atoms List
	}

	public static void addLTLFFormula(String ltl_formula) {
		String constraint = "LTL{" + ltl_formula + "}";
		Constants.getConstraintsPerspective().getConstraintsListModel().addElement(constraint);
	}

}
