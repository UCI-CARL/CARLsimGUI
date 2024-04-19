

Models: 

1. Maze
------- 

Maze is the data structure that holds the traversal costs 
and provides an interface to conventiently use it.
For instance it provides the transformation of 

(row,column) coordinates to index , as to 3D points of SpikeStream given by (x,y,z)


index <-> (row,column) <-> (x,y,z) 




2. ExpCarlsimWrapper
---------------------------

Provides access to underlying Simulation running in CARLsim. 

For instance 

getting the Spikes of the SpikeMonitor on the PlaceCells 
utilized for Spike

or getting the firing sequence in.. a  ...

This logic must go somewhere and should not be in the ExperiementManager
that is build to provide the interaction with the user (MVC).

 
 


 3. Results Cursor
 -----------------
 
                
 /*
			QString // Experiment, Name, Network, push   sub 

			Container:		results\\					
			Experiment:			E-Prop_Morris_2022-07-12T0040\\		Class, Experiment Name, TimeStamp
			Phase:					1_Train\\						Number and Name, to ensure file sorting
			Loop:					    Loop_1\\					Repeats the same 
			Iteration:						Segment_0\\				Iterates over a collection, or Random walk
			
			at each level the results of the level below can be referenced and summarized (e.g. loos / loop)
			Fig. 13 with 20 sub figures,  which is to be generated with a matlab template
			Fig 14   24 xxx.   -> Template:  placeholder are filled by Qt  reg exp,  
				avoid writing a new report generator !!!  => only as parameter .. 
			Matlab script is called from CARLsim GUI to generate .png   (system, CLI, parameters  e.g. loops, segments, 
			
			Utility Class "ResultsCurser" is ustified: 
			- Can hold a status (->cursor) that is set somewhere and reused at multiple xx 
			- Eases write oprations (only file name, cursor, options e.g current and/or commulativ flags)
			- Encapsulate File operations e.g. the directory creation at first usage
			- TimeStamp as FileName
			- Can be UnitTested
			- Provides a proper interface and encapsulates the naming 


     expWrapper->writePath(resultsCursor.spawn("path.txt"));  // current and cummulative experiment
     
     
     creates a tree (parent, childs )  hirachy 
     
     ResultsCursor results(container = "results");   // default
     auto experimentResults = results.spanExperiment("E-Prop", name)   // time stamp is generated
     auto phaseResults = experimentResults.spanDir(expNo, "train");   // experiment_no
     auto loopResults = phaseResults.spanLoop(loop)  or   spanDir(QString::asprintf("%d_train", expNo))
     auto iterResults = loopResults.spanIteration(segment_i)   or  spanDir(QString::asprintf("segment_%d", segment_i)) 
     auto fileResults = iterResults.spanFile()
     spanFile()


Implementation:

  
  QFile     
  
  QDir 
  
  QFileInfo  -->  works togethter with QFile QDir and seems
              to have most  
    
  
  QRessource  -> No,  isDir(),    is protected
    absoluteFileName()  OK 
   
 
 NOTE 
 https://doc.qt.io/qt-6/threads-reentrancy.html

*/




