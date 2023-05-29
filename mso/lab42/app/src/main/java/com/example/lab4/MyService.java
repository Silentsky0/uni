package com.example.lab4;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;
import android.os.Process;
import android.os.RemoteException;
import android.util.Log;

public class MyService extends Service {
    public MyService() {
    }
    private Looper mServiceLooper;
    private ServiceHandler mServiceHandler;
    private Messenger mMessenger;

    private final class ServiceHandler extends Handler {
        public ServiceHandler(Looper looper) {
            super(looper);
        }
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 432) {
                Log.i("serviceHandler", msg.getData().toString());
                int data = msg.getData().getInt("data");

                Messenger replyTo = msg.replyTo;

                Message reply = Message.obtain(null, 433, 0, 0);
                Bundle dataBundle = new Bundle();
                dataBundle.putInt("result", data * data);
                reply.setData(dataBundle);
                try {
                    replyTo.send(reply);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
            }
            if (msg.what == 434) {
                Log.i("serviceHandler", msg.getData().toString());
                int data = msg.getData().getInt("data");

                Messenger replyTo = msg.replyTo;

                Message reply = Message.obtain(null, 435, 0, 0);
                Bundle dataBundle = new Bundle();
                dataBundle.putInt("result", data * data * data);
                reply.setData(dataBundle);
                try {
                    replyTo.send(reply);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    @Override
    public void onCreate() {
        HandlerThread thread = new HandlerThread("MyService", Process.THREAD_PRIORITY_BACKGROUND);
        thread.start();
        mServiceLooper = thread.getLooper();
        mServiceHandler = new ServiceHandler(mServiceLooper);
        mMessenger = new Messenger(mServiceHandler);

        Log.i("myService", "Service started");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Message msg = mServiceHandler.obtainMessage();
        msg.arg1 = startId;
        mServiceHandler.sendMessage(msg);
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i("myService", "Service stopped");
    }
}