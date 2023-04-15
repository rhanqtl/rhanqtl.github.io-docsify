package main

import "sync"

func main() {
	var channels []chan int
	for i := 0; i < 5; i++ {
		channels = append(channels, make(chan int))
	}

	var wg sync.WaitGroup
	wg.Add(6)

	go func() {
		out := channels[0]
		for i := 2; i <= 11; i++ {
			out <- i
		}
		close(out)
		wg.Done()
	}()
	for i := 0; i < 5; i++ {
		var out chan int
		if i != 4 {
			out = channels[i+1]
		} else {
			out = nil
		}
		go func(in <-chan int, out chan<- int) {
			p := <-in
			println(p)
			for n := range in {
				if n%p != 0 && out != nil {
					out <- n
				}
			}
			if out != nil {
				close(out)
			}
			wg.Done()
		}(channels[i], out)
	}

	wg.Wait()
}
