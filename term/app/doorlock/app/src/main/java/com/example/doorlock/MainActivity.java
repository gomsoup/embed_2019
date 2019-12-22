package com.example.doorlock;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.charset.StandardCharsets;

public class MainActivity extends AppCompatActivity {
    EditText textpw;
    TextView temppw;
    Button changePwBtn;
    Button getTempPwBtn;
    byte[] clos = "CLOS".getBytes(StandardCharsets.US_ASCII);

    byte[] temp = new byte[4];

    public boolean isPassword(String str){
        if(str.length() != 4) return false;

        for(int i = 0; i < str.length(); i++){
            char c = str.charAt(i);
            if(!Character.isDigit(c)) return false;
        }

        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        FirebaseMessaging.getInstance().subscribeToTopic("com_example_doorlock")
                .addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        String msg = getString(R.string.app_name);
                        if (!task.isSuccessful()) {
                            msg = getString(R.string.app_name);
                        }
                        Log.d("hida", msg);
                    }
                });

        final EditText textpw = (EditText) findViewById(R.id.edittext);
        final TextView temppw = (TextView) findViewById(R.id.temppw);
        Button changePwBtn = (Button) findViewById(R.id.changepwbtn);
        Button getTempPwBtn = (Button) findViewById(R.id.temppwbtn);

        changePwBtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                String str = textpw.getText().toString();

                if(!isPassword(str)) {
                    Toast.makeText(MainActivity.this, "Password must be 4 digits", Toast.LENGTH_SHORT).show();
                    return;
                }

                try{
                    BackgountTask bg = new BackgountTask();
                    String buf = "UDAT";

                    String ret = bg.execute(buf, str).get();
                    if(ret.equals("fail")) Toast.makeText(MainActivity.this, "Server is now up. But something wrong in communication.", Toast.LENGTH_SHORT).show();
                    else Toast.makeText(MainActivity.this, "Success", Toast.LENGTH_SHORT).show();

                    bg.cancel(true);
                }catch (Exception e){
                    Toast.makeText(MainActivity.this, "Something wrong in socket communication", Toast.LENGTH_SHORT).show();
                }
            }
        });

        getTempPwBtn.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                try{
                    BackgountTask bg = new BackgountTask();
                    String buf = "TEMP";

                    String ret = bg.execute(buf).get();
                    if(ret.equals("fail")) Toast.makeText(MainActivity.this, "Server is now up. But something wrong in communication.", Toast.LENGTH_SHORT).show();
                    else Toast.makeText(MainActivity.this, "Success", Toast.LENGTH_SHORT).show();

                    temppw.setText(new String(ret));

                    bg.cancel(true);
                }catch (Exception e){
                    Toast.makeText(MainActivity.this, "Something wrong in socket communication", Toast.LENGTH_SHORT).show();
                }
            }
        });

    }

    class BackgountTask extends AsyncTask<String, String, String> {
        Socket s = null;
        InputStream is = null;
        OutputStream os = null;


        public void connectSocket(){
            try{
                s = new Socket("server.gomsoup.com", 4000);

                is = s.getInputStream();
                os = s.getOutputStream();
            }catch(Exception e){
             //   Toast.makeText(MainActivity.this, "Socket create failed. Server might be down.", Toast.LENGTH_SHORT).show();
                e.printStackTrace();
            }
        }


        @Override
        protected String doInBackground(String... str) {
            if(is == null || os == null){
             //   Toast.makeText(MainActivity.this, "Server Down", Toast.LENGTH_SHORT).show();
                return null;
            }

            if(str.length > 1){
                try {
                    byte[] buf = str[0].getBytes(StandardCharsets.US_ASCII);
                    os.write(buf, 0, 4);

                    buf = str[1].getBytes(StandardCharsets.US_ASCII);
                    os.write(buf, 0, 4);
                }catch (Exception e) {
                    e.printStackTrace();
                    return "fail";
                }
            }
            else{
                try {
                    byte[] buf = str[0].getBytes(StandardCharsets.US_ASCII);
                    os.write(buf, 0, 4);

                    is.read(buf);
                    return new String(buf, "UTF-8");
                }catch (Exception e) {
                    e.printStackTrace();
                    return "fail";
                }
            }

            return "ok";
        }

        protected void onPreExecute(){
            connectSocket();
        }

        protected void onPostExecute(String result){
            if(s != null){
                try {
                    os.write(clos, 0, 4);

                    is.close();
                    os.close();
                    s.close();
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
        }

        protected void onCancelled() {
            if(s != null){
                try {
                    os.write(clos, 0, 4);

                    is.close();
                    os.close();
                    s.close();
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
        }

    }

}
