package com.hub.demo;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class cket {
    public void open() throws UnknownHostException, IOException {
    	Socket socket=new Socket("localhost",10086);
    	 OutputStream os=socket.getOutputStream();//�ֽ������
         PrintWriter pw=new PrintWriter(os);//���������װΪ��ӡ��
         pw.write("1");
         pw.flush();
         pw.close();
         os.close();
         socket.close();
    }
    public void close() throws UnknownHostException, IOException {
    	Socket socket=new Socket("localhost",10086);
    	 OutputStream os=socket.getOutputStream();//�ֽ������
         PrintWriter pw=new PrintWriter(os);//���������װΪ��ӡ��
         pw.write("2");
         pw.flush();
         pw.close();
         os.close();
         socket.close();
    }
}