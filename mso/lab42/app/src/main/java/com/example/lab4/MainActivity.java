package com.example.lab4;

import androidx.appcompat.app.AppCompatActivity;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.google.android.material.textfield.TextInputEditText;

public class MainActivity extends AppCompatActivity {

    private final class ReplyHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 433) {
                Log.i("replyHandler square", msg.getData().toString());
                int result = msg.getData().getInt("result");

                TextView serviceResult = findViewById(R.id.textView);
                serviceResult.setText(String.valueOf(result));
                serviceResult.setVisibility(View.VISIBLE);
            }
            if (msg.what == 435) {
                Log.i("replyHandler cube", msg.getData().toString());
                int result = msg.getData().getInt("result");

                TextView serviceResult = findViewById(R.id.textView);
                serviceResult.setText(String.valueOf(result));
                serviceResult.setVisibility(View.VISIBLE);
            }
        }
    }

    private ReplyHandler replyHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Intent startMyService = new Intent(this, MyService.class);
        startService(startMyService);

        Intent messenger = new Intent();
        messenger.setClassName("com.example.lab4", "com.example.lab4.MyService");
        bindService(messenger, serviceConnection, Context.BIND_AUTO_CREATE);


        // UI
        Button calculateSquareButton = findViewById(R.id.button);
        Button calculateCubeButton = findViewById(R.id.button2);

        TextView serviceResult = findViewById(R.id.textView);
        serviceResult.setVisibility(View.INVISIBLE);

        TextInputEditText textInputEditText = findViewById(R.id.my_text_input);
        calculateSquareButton.setOnClickListener(v -> {
            if (isBound) {
                Message msg = Message.obtain(null, 432, 0, 0);
                Bundle dataBundle = new Bundle();

                if (textInputEditText.getText().toString().equals("")) {
                    return;
                }

                dataBundle.putInt("data", Integer.parseInt(textInputEditText.getText().toString()));
                msg.setData(dataBundle);
                msg.replyTo = replyMessenger;

                try {
                    mMessenger.send(msg);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

        });
        calculateCubeButton.setOnClickListener(v -> {
            if (isBound) {
                Message msg = Message.obtain(null, 434, 0, 0);
                Bundle dataBundle = new Bundle();
                if (textInputEditText.getText().toString().equals("")) {
                    return;
                }

                dataBundle.putInt("data", Integer.parseInt(textInputEditText.getText().toString()));
                msg.setData(dataBundle);
                msg.replyTo = replyMessenger;

                try {
                    mMessenger.send(msg);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    boolean isBound = false;
    Messenger mMessenger = null;
    Messenger replyMessenger = null;
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            isBound = true;
            mMessenger = new Messenger(service);
            replyHandler = new ReplyHandler();
            replyMessenger = new Messenger(replyHandler);
            Log.i("serviceConnection", "Service connected");
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mMessenger = null;
            replyMessenger = null;
            isBound = false;
            Log.i("serviceConnection", "Service disconnected");
        }
    };


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == 444 && resultCode == RESULT_OK) {
            Log.d("MyService", "Result is OK: " + data.getStringExtra("result"));

            final TextView serviceResult = findViewById(R.id.textView);
            serviceResult.setText("Result: " + data.getStringExtra("result"));
            serviceResult.setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onBackPressed() {
        unbindService(serviceConnection);

        super.onBackPressed();
    }
}