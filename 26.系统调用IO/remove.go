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

		// 先读6字节，应该是aaaaa\n
		buf := make([]byte, 6)
		readn, err := f.Read(buf)
		if err != nil {
			fmt.Printf("Read2 file error: %v\n", err)
			return
		}
		fmt.Printf("read2 %v bytes\n", readn)
		fmt.Println(string(buf))

		select {
		case <-ch:
			// 此时文件已经被main routine已经删除，还能继续读取吗？
			buf = make([]byte, 6)
			readn, err = f.Read(buf)
			if err != nil {
				fmt.Printf("[Select] Read2 file error: %v\n", err)
				return
			}
			fmt.Printf("[Select] read2 %v bytes\n", readn)
			fmt.Println(string(buf))

			// 此时文件已经被删除，还能继续写入吗？
			n, err := f.Write([]byte("111"))
			if err != nil {
				fmt.Println("[Select] write error:", err)
				return
			}
			fmt.Printf("[Select] write2 %v bytes\n", n)
		}
	}(ch, &wg)

	// main routine读取6字节
	buf := make([]byte, 6)
	readn, err := f.Read(buf)
	if err != nil {
		fmt.Printf("Read file error: %v\n", err)
		return
	}
	fmt.Printf("read %v bytes\n", readn)
	fmt.Println(string(buf))

	time.Sleep(10 * time.Second)


	// 关闭并删除文件
	if err := f.Close(); err != nil {
		fmt.Println("Close file err:", err)
		return
	}

	// 删除或重命名文件
	//if err := os.Remove("/tmp/zhang/nn.txt"); err != nil {
	if err := os.Rename("/tmp/zhang/nn.txt", "/tmp/zhang/nn_rename.txt"); err != nil {
		fmt.Println("Remove file err:", err)
		return
	}
	fmt.Println("file has be removed")

	time.Sleep(10 * time.Second)

	// 写完后发送信号，让goroutine去再读或写
	ch <- struct{}{}
	wg.Wait()

	fmt.Println("main routine end")

}
