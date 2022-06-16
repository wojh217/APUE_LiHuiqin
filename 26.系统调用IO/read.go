package main

import (
	"fmt"
	"os"
	"sync"
	"time"
)

func main() {
	// nn.txt内容为aaaaa\nbbbbb\nccccc\nddddd\nfffff\n
	f, err := os.OpenFile("/tmp/zhang/nn.txt", os.O_RDWR, 0666)
	if err != nil {
		fmt.Printf("Open file error: %v\n", err)
		return
	}
	defer f.Close()

	ch := make(chan struct{}, 1)
	var wg sync.WaitGroup
	// 每次启动goroutine前wg加一
	wg.Add(1)
	go func(ch chan struct{}, wg *sync.WaitGroup) {
		defer wg.Done()
		f, err := os.OpenFile("/tmp/zhang/nn.txt", os.O_RDWR, 0666)
		if err != nil {
			fmt.Printf("Open2 file error: %v\n", err)
			return
		}
		defer f.Close()

		time.Sleep(1 * time.Second)


		// 先读18字节，应该是aaaaa\nbbbbb\nccccc\n
		buf := make([]byte, 18)
		readn, err := f.Read(buf)
		if err != nil {
			fmt.Printf("Read2 file error: %v\n", err)
			return
		}
		fmt.Printf("read2 %v bytes\n", readn)
		fmt.Println(string(buf))

		select {
		case <-ch:
			f.Seek(0, 0)
			// 再读18字节，看是否是main goroutine修改后的内容
			buf = make([]byte, 18)
			_, err = f.Read(buf)
			if err != nil {
				fmt.Printf("[Select] Read2 file error: %v\n", err)
			}

		}
	}(ch, &wg)

	ret, err := f.Seek(6, 0)
	if err != nil {
		fmt.Printf("Seek file error: %v\n", err)
		return
	}
	fmt.Printf("ret: %v\n", ret)

	buf := make([]byte, 6)
	readn, err := f.Read(buf)
	if err != nil {
		fmt.Printf("Read file error: %v\n", err)
		return
	}
	fmt.Printf("read %v bytes\n", readn)
	fmt.Println(string(buf))

	// 第三行写为33333
	n, err := f.Write([]byte("33333\n"))
	if err != nil {
		fmt.Printf("Write file error: %v\n", err)
		return
	}
	// 写完后发送信号，让goroutine去再读
	ch <- struct{}{}
	wg.Wait()


	fmt.Printf("Write %v bytes\n", n)


}
