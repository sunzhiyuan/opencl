***************************************
* How to run Adreno Profiler on Linux *
***************************************

Requirements:
	Mono Framework 2.10.8 or above
		For older Ubuntu releases such as Lucid(10.4) the package sources 
		need to be updated with http://badgerports.org/.
		Run the following command to install Mono:
			$ sudo apt-get install mono-complete
		Please verify the framework requirement by checking the Mono version with:
			$ mono --version
	If using Adreno Profiler with Android make sure your environment is set
	  with the Android SDK and adb is in your PATH library.
  
Run Adreno Profiler:
	Double click application within the file explorer or through command line:
		$ ./AdrenoProfiler.exe 
		or
		$ mono AdrenoProfiler.exe