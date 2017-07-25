package main

import (
	"log"
	"net"
	"sync"
	"time"
)

func main() {
	var wg sync.WaitGroup

	for i := 0; i < 1000; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()

			conn, err := net.Dial("tcp", "127.0.0.1:8000")
			if err != nil {
				log.Fatalln("Dial", err)
			}

			defer conn.Close()

			go func() {
				for {
					buff := make([]byte, 128)
					_, err := conn.Read(buff)
					if err != nil {
						log.Println("Read:", err)
						continue
					}
					log.Printf("I receiver:%s\n", buff)
				}
			}()

			for {
				_, err := conn.Write([]byte("My time is:" + time.Now().String()))
				if err != nil {
					log.Println("Write", err)
				}
				time.Sleep(1 * time.Second)
			}
		}()
	}
	wg.Wait()
	log.Println("done")
}
