package com.example.doorlock;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.FirebaseInstanceIdService;
import com.google.firebase.messaging.FirebaseMessaging;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class MyFirebaseInstanceIDService extends FirebaseInstanceIdService {
    private final String TAG="your_tag";

    @Override
    public void onTokenRefresh() {
        String refreshedToken = FirebaseInstanceId.getInstance().getToken();
        Log.d("IDService", "Refreshed token: " + refreshedToken);

        FirebaseMessaging.getInstance().subscribeToTopic("com_example_doorlock");
        sendRegistrationToServer(refreshedToken);
    }

    private void sendRegistrationToServer(String token) {

    }
}

