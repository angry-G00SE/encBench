package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path"
	"regexp"
	"runtime"
	"strings"
)

func main() {
	_, filename, _, _ := runtime.Caller(0)
	dir := path.Dir(filename)
	os.Chdir(dir)

	algorithm := flag.String("algorithm", "", "")
	iterations := flag.String("iterations", "", "")
	time := flag.String("time", "", "")
	size := flag.String("size", "", "")
	threads := flag.String("threads", "", "")

	flag.Parse()

	args := []string{
		"test",
		"-bench=" + *algorithm + "/" + *size + "$",
		"-benchmem",
		"-cpu=" + *threads,
	}

	var benchtime string
	if *time == "" {
		benchtime = *iterations + "x"
	} else {
		benchtime = *time + "s"
	}

	args = append(args, "-benchtime="+benchtime)

	// execute the benchmark with the given settings
	data, err := exec.Command("go", args...).Output()
	checkError(err)

	raw := string(data)

	metrics := make([]string, 0)

	metrics = append(metrics, find(raw, "goos: (.*)"))
	metrics = append(metrics, find(raw, "goarch: (.*)"))
	metrics = append(metrics, find(raw, "cpu: (.*)"))
	metrics = append(metrics, find(raw, "ok\\s*\\S*\\s*(\\S*)s"))
	metrics = append(metrics, find(raw, "(\\S*) allocs/op"))
	metrics = append(metrics, find(raw, "(\\S*) B/op"))
	metrics = append(metrics, find(raw, "(\\S*) ns/op"))
	metrics = append(metrics, find(raw, "(\\S*)\\s*\\S* ns/op"))

	// os:architecture:cpu:time:allocations:bytes:avergae:iterations
	output := strings.Join(metrics, "\n")
	fmt.Println(output)
}

func checkError(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func find(target, pattern string) string {
	re := regexp.MustCompile(pattern)
	return re.FindStringSubmatch(target)[1]
}
