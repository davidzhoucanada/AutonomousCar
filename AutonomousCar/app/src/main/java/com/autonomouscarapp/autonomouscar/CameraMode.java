package com.autonomouscarapp.autonomouscar;

import android.annotation.TargetApi;
import android.support.annotation.NonNull;
import android.support.annotation.RequiresApi;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.content.pm.PackageManager;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraDevice.StateCallback;
import android.hardware.camera2.CameraManager;
import android.os.Handler;

import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import android.widget.Toast;

@RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
public class CameraMode extends AppCompatActivity {

    Button takePictureButton;
    ImageView pictureTaken;

    private static final int CAMERA_PERMISSION_REQUEST_CODE = 8888;
    private String cameraId = "0";
    private CameraDevice backCamera = null;
    private Handler eventHandler;

    // Runs different functions based on state of camera
    private final StateCallback backCameraStateCallback = new StateCallback() {

        @Override
        public void onOpened(@NonNull CameraDevice camera) {
            backCamera = camera;
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice camera) {
            camera.close();
            backCamera = null;
        }

        @Override
        public void onError(@NonNull CameraDevice camera, int error) {
            camera.close();
            backCamera = null;
            message("Camera Failed");
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_mode);

        takePictureButton = (Button) findViewById(R.id.takePictureButton);
        pictureTaken = (ImageView) findViewById(R.id.pictureTaken);

        takePictureButton.setOnClickListener(takePictureButtonClickListener);
    }

    private void message(String s) {
        final Toast toastMessage = Toast.makeText(getApplicationContext(), s, Toast.LENGTH_SHORT);
        toastMessage.show();

        // Shortens duration of Toast message
        // Taken from https://stackoverflow.com/questions/6094792/how-to-set-toast-display-time-less-than-toast-length-short, by user Erikas
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                toastMessage.cancel();
            }
        }, 1000);
    }

    @TargetApi(Build.VERSION_CODES.M)
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void openBackCamera() {
        CameraManager cameraManager = (CameraManager) getSystemService(CAMERA_SERVICE);
        if (checkSelfPermission(android.Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            requestCameraPermission();
        }
        try {
            cameraManager.openCamera(cameraId, backCameraStateCallback, eventHandler);
        } catch (Exception e) {
            message("Cannot Open Camera");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    private void requestCameraPermission() {
        String[] permissionRequest = {android.Manifest.permission.CAMERA};
        requestPermissions(permissionRequest, CAMERA_PERMISSION_REQUEST_CODE);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            } else {
                message("No permission - can't take photo");
            }
        }
    }

    private View.OnClickListener takePictureButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            openBackCamera();
        }
    };
}

