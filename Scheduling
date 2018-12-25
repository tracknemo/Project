
package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"os"
	"regexp"
	"sort"
	"strconv"
	"strings"
)

type process struct {
	Arrival    int    
	Burst      int    
	Wait       int    
	Turnaround int 
	PausedAt   int
	RunTime    int          
	Finished   bool  
	HasArrived bool    
	Name       string  
}

type by func(p1, processCountTwo *process) bool

func (by by) Sort(processes []*process) {
	ps := &processSorter{
		processes: processes,
		by:        by,
	}
	sort.Sort(ps)
}

type processSorter struct {
	processes []*process
	by        func(p1, processCountTwo *process) bool
}

func (s *processSorter) Swap(i, j int) {
	s.processes[i], s.processes[j] = s.processes[j], s.processes[i]
}

func (s *processSorter) Less(i, j int) bool {
	return s.by(s.processes[i], s.processes[j])
}

func (s *processSorter) Len() int {
	return len(s.processes)
}


var output *bufio.Writer

func main() {

	var (
		err           error      
		runfor        int        
		alg           string     
		quantum       int    
		processCount   int            
		processes     []*process 
		origProcesses []*process 
	)

	// read from the specified file
	input, err := ioutil.ReadFile(os.Args[1])
	if err != nil {
		fmt.Println("Error: Could not open file")
	}

	// write to a file that the professor will indicate
	outputFile, err := os.Create(os.Args[2])
	if err != nil {
		fmt.Println("Error: Could not create write file")
	}

	output = bufio.NewWriter(outputFile)
	defer outputFile.Close()
	defer output.Flush()

	var re = regexp.MustCompile("\\s+")

	inputArr := strings.Split(string(input), "\n")
	cleanInput := ""
	for _, s := range inputArr {
		sClean := strings.TrimSpace(s)
		sClean = re.ReplaceAllString(sClean, " ")

		if strings.HasPrefix(sClean, "#") { // String starts with #
			continue
		} else if strings.Index(sClean, "#") > 0 { // String contains #
			cleanInput += sClean[:strings.Index(sClean, "#")]
		} else { // Just append
			cleanInput += sClean
		}
		cleanInput += "\n"
	}

	// parse the input
	for _, s := range strings.Split(cleanInput, "\n") {
		splt := strings.Split(s, " ")

		// Get process count
		switch splt[0] {

		case "runfor":
			runfor, err = strconv.Atoi(splt[1])
			break

		case "processcount":
			processCount, err = strconv.Atoi(splt[1])
			break

		case "quantum":
			quantum, err = strconv.Atoi(splt[1])
			break

		case "use":
			alg = splt[1]
			break

		case "process":
			var currProcess process
			currProcess.Finished = false

			for index, name := range splt {
				switch name {
				case "name":
					currProcess.Name = splt[index+1]
					break
				case "burst":
					currProcess.Burst, err = strconv.Atoi(splt[index+1])
					break
				case "arrival":
					currProcess.Arrival, err = strconv.Atoi(splt[index+1])
					break
				}
			}

			processes = append(processes, &currProcess)
			origProcesses = append(origProcesses, &currProcess)
			break
		}

		// Error check
		if err != nil {
			fmt.Print("error, the specified input file could not be parsed")
			fmt.Println(err)
			return
		}
	}

	// Sort by the arrival of the process
	by(func(p1, processCountTwo *process) bool {
		return p1.Arrival < processCountTwo.Arrival
	}).Sort(processes)

	//run the specified algorithm
	switch alg {
	case "sjf":
		sjf(processCount, runfor, quantum, processes, origProcesses)
		break
	case "fcfs":
		fcfs(processCount, runfor, quantum, processes, origProcesses)
		break
	case "rr":
		rr(processCount, runfor, quantum, processes, origProcesses)
		break
	}
}

// first come first serve algorithm
func fcfs(processCount int, runfor int, quantum int, processes []*process, origProcesses []*process) {

	// print the header info to the user
	fmt.Println("%3d processes\nUsing First-Come First-Served\n", processCount)

	time := 0

	// Just process them as they stand (already sorted by arrival)
	for _, currentProcess := range processes {

		if time < currentProcess.Arrival {
			outputIdleProcs(time, currentProcess.Arrival-1)
			time = currentProcess.Arrival
		}

		if time == currentProcess.Arrival {
			outputArrival(currentProcess)
		}
		outputSelectedTimes(time, currentProcess)

		for _, processCountTwo := range arrivalBetween(time, time+currentProcess.Burst, processes) {
			outputArrival(processCountTwo)
		}

		currentProcess.Wait = time - currentProcess.Arrival
		currentProcess.Turnaround = currentProcess.Wait + currentProcess.Burst
		currentProcess.Finished = true
		time += currentProcess.Burst

		// ensure you don't go over the time limit
		if time > runfor {
			break
		}

		// Finish process
		outputFinishedProcs(time, currentProcess)
	}

	// for the rest of the time remaining, stay idle
	if time < runfor {
		outputIdleProcs(time, runfor-1)
	}

	// Finalize
	outputFinalInfo(runfor, origProcesses)
}

// shortest job first algorithm
func sjf(processCount int, runfor int, quantum int, processes []*process, origProcesses []*process) {

	// print the header info to the user
	fmt.Println("%3d processes\nUsing preemptive Shortest Job First\n", processCount)

	var running *process

	for time := 0; time < runfor; time++ {

		// Get all processes that have arrived
		possible := arrivalBetween(-1, time, processes)

		// Sort by Priorty (Time remaining)
		by(func(p1, processCountTwo *process) bool {
			return p1.Burst-p1.RunTime < processCountTwo.Burst-processCountTwo.RunTime
		}).Sort(possible)

		// Find new shortest job (thats not done)
		var currentProcess *process
		for i := 0; i < len(possible); i++ {

			// out put the new arrivals to the user
			if possible[i].Arrival == time && !possible[i].HasArrived {
				outputArrival(possible[i])
				possible[i].HasArrived = true
			}

			// take the first non finished process
			if !possible[i].Finished && currentProcess == nil {
				currentProcess = possible[i]
			}
		}

		
		if currentProcess == nil {
			outputIdleProcs(time, time)
			continue
		}

		// check to see if the curr job is complete, if not continue
		if currentProcess == running {
			if currentProcess.RunTime >= currentProcess.Burst {
				currentProcess.Turnaround = time - currentProcess.Arrival
				currentProcess.Finished = true
				running = nil
				outputFinishedProcs(time, currentProcess)
				time--
			} else {
				currentProcess.RunTime++
			}
			continue
		}

		if running != nil {
			running.PausedAt = time
		}
		running = currentProcess
		outputSelectedTimes(time, running)

		if running.RunTime > 0 {
			running.Wait += time - running.PausedAt
		} else {
			running.Wait += time - running.Arrival
		}

		running.RunTime++

	}

	// output the final info for the given task
	outputFinalInfo(runfor, origProcesses)
}

//round robin algorithm
func rr(processCount int, runfor int, quantum int, processes []*process, origProcesses []*process) {

	fmt.Println("%3d processes\nUsing Round-Robin\nQuantum %3d\n\n", processCount, quantum)

	// Process queue & add first arrival
	var queue []*process
	queue = append(queue, processes[0])
	outputArrival(queue[0])

	time := queue[0].Arrival

	// process each in the order of arruval
	for len(queue) > 0 {
		var currentProcess *process
		currentProcess, queue = queue[0], queue[1:]

		// output to the user the idle processes
		if time < currentProcess.Arrival {
			outputIdleProcs(time, currentProcess.Arrival-1)
			time = currentProcess.Arrival
		}

		// in this step we are calculating the runtime
		newTimeValue := quantum
		if currentProcess.RunTime+quantum > currentProcess.Burst {
			newTimeValue = currentProcess.Burst - currentProcess.RunTime
		}

		if time+newTimeValue > runfor {
			newTimeValue = runfor - time
		}

		outputSelectedTimes(time, currentProcess)
		currentProcess.RunTime += newTimeValue

		//add additional wait time to every process except for the current
		for i := 0; i < len(queue); i++ {
			queue[i].Wait += newTimeValue
		}

		// if anything arrived durring the current run, add it the the queue
		for _, processCountTwo := range arrivalBetween(time, time+newTimeValue, processes) {
			outputArrival(processCountTwo)
			queue = append(queue, processCountTwo)

			processCountTwo.Wait += time + newTimeValue - processCountTwo.Arrival
		}

		// go to the next point in time dpeending on the current position you are in
		time += newTimeValue

		// ensure that the process has finished
		if currentProcess.RunTime >= currentProcess.Burst {
			outputFinishedProcs(time, currentProcess)
			currentProcess.Turnaround = time - currentProcess.Arrival
			currentProcess.Finished = true
		} else {
			queue = append(queue, currentProcess)
		}

	}

	// remain idle until time is up
	if time < runfor {
		outputIdleProcs(time, runfor-1)
	}
	// output the final info to the user
	outputFinalInfo(runfor, origProcesses)
}

// retrieve all the processes that arrived between a specific start and end time
func arrivalBetween(start int, end int, processes []*process) []*process {
	var arrived []*process

	for _, currentProcess := range processes {
		if currentProcess.Arrival > start && currentProcess.Arrival <= end {
			arrived = append(arrived, currentProcess)
		}
	}
	return arrived
}

// output to the user the idle processes
func outputIdleProcs(start int, end int) {
	for i := start; i <= end; i++ {
		fmt.Println("Time %3d : Idle\n", i)
	}
}

// Function prints the arrival time
func outputArrival(process *process) {
	fmt.Println("Time %3d : %s arrived\n", process.Arrival, process.Name)
}

// output to the user the finished process
func outputFinishedProcs(time int, process *process) {
	fmt.Println("Time %3d : %s finished\n", time, process.Name)
}

// output to the user the selected time
func outputSelectedTimes(time int, process *process) {
	fmt.Println("Time %3d : %s selected (burst %3d)\n", time, process.Name, process.Burst-process.RunTime)
}

// output to the user the final info
func outputFinalInfo(time int, processes []*process) {

	fmt.Println("Finished at time %3d\n\n", time)

	for _, currentProcess := range processes {
		fmt.Println("%s wait %3d turnaround %3d\n", currentProcess.Name, currentProcess.Wait, currentProcess.Turnaround)
	}
}
