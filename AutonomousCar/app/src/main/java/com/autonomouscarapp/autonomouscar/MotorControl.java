package com.autonomouscarapp.autonomouscar;
import android.os.Handler;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;

// Visual Libraries
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

// Messaging Libraries
import android.widget.RelativeLayout;
import android.widget.Toast;

// Bluetooth Libraries
import android.bluetooth.BluetoothSocket;

// Control Libraries
import android.view.MotionEvent;

public class MotorControl extends AppCompatActivity { // Implement statement is so switch onClick can be used

    // Variable Declaration

    // Visuals
    ImageView controlWheelOuter, controlWheelInner;
    RelativeLayout controlWheelBox;
    ConstraintLayout screen;

    // Bluetooth Connection
    private BluetoothConnection myBluetoothConnection;
    BluetoothSocket myBluetoothSocket = null;
    String MACAddress;

    // Control Wheel
    private float screenHeight, padding;
    private float controlWheelOuterRadius, controlWheelInnerRadius;
    private float originalX, originalY, originalCenterX, originalCenterY;
    float x, y;
    double touchRadius;
    int leftRight, forwardBackward, speed;
    String speedString;


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        // Set up layout
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_motor_control);

        // Creates a new Bluetooth Connection
        myBluetoothConnection = new BluetoothConnection();

        // Gets the MAC Address of the desired device
        MACAddress = getIntent().getExtras().getString("device_address");

        // Sends a message to show it's connecting, then connects (make new connect thread, make RFCOMM connection)
        message("Connecting...");
        myBluetoothConnection.connect(myBluetoothConnection.getDevice(MACAddress));
        myBluetoothSocket = myBluetoothConnection.getMyBluetoothSocket();

        // View initialization
        screen = (ConstraintLayout)findViewById(R.id.screen);
        controlWheelBox = (RelativeLayout)findViewById(R.id.controlWheelBox);
        controlWheelOuter = (ImageView)findViewById(R.id.controlWheelOuter);
        controlWheelInner = (ImageView)findViewById(R.id.controlWheelInner);
        controlWheelInner.setOnTouchListener(controlWheelInnerTouchListener);

        // Default value of leftRight, car does not go left or right, only forwards/backwards; left = 2, right = 3
        leftRight = 0;

        // Sets constant values of control wheel after control wheel view has been set up
        controlWheelBox.post(new Runnable() {
            @Override
            public void run() {
                screenHeight = screen.getHeight();
                controlWheelOuterRadius = controlWheelBox.getWidth()/2;
                controlWheelInnerRadius = controlWheelInner.getWidth()/2;
                padding = (screenHeight - controlWheelOuterRadius)/2;
                originalX = controlWheelOuterRadius - controlWheelInnerRadius;
                originalY = controlWheelOuterRadius - controlWheelInnerRadius;
                originalCenterX = originalX + controlWheelInnerRadius;
                originalCenterY = originalY + controlWheelInnerRadius;
            }
        });
    }

    // Creates a small popup message
    private void message(String s)
    {
        final Toast toastMessage = Toast.makeText(getApplicationContext(),s,Toast.LENGTH_SHORT);
        toastMessage.show();

        // Shortens duration of Toast message
        // Taken from https://stackoverflow.com/questions/6094792/how-to-set-toast-display-time-less-than-toast-length-short, by user Erikas
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                toastMessage.cancel();
            }
        }, 500);
    }

    // Touch listener for inner control wheel
    private View.OnTouchListener controlWheelInnerTouchListener = new View.OnTouchListener() {

        public boolean onTouch(View v, MotionEvent event) {

            // Obtain the coordinates of finger
            x = event.getRawX();
            y = event.getRawY();

            switch (event.getAction()) {

                // Moment inner control wheel is touched
                case MotionEvent.ACTION_DOWN:
                    synchronized (this) {
                    }
                    break;

                // Dragging inner control wheel around
                case MotionEvent.ACTION_MOVE:
                    synchronized (this) {
                        touchRadius = Math.sqrt(Math.pow(x-originalCenterX,2) + Math.pow(y-padding-originalCenterY,2));

                        // Moves inner control wheel if touch is within bounds
                        if (touchRadius <= controlWheelOuterRadius - controlWheelInnerRadius){
                            controlWheelInner.setX(x - controlWheelInnerRadius);
                            controlWheelInner.setY(y - padding - controlWheelInnerRadius);
                        }

                        // Otherwise sets max touch radius so that car continues to move
                        else {
                            touchRadius = controlWheelOuterRadius - controlWheelInnerRadius;
                        }

                        speed = (int)((touchRadius/(controlWheelOuterRadius-controlWheelInnerRadius))*255);

                        // Adding 1000 to get leading zeros if speed is 1 - e.g., if speed = 1, speed + 1000 = 1001, substring taken is 001
                        speedString = String.valueOf(speed+1000).substring(1);

                        // If control wheel is at least 25 pixels to right of center of outer wheel, leftRight is 3, car goes right
                        if (x - originalCenterX > 25){
                            leftRight = 3;
                        }

                        // If control wheel is at least 25 pixels to left of center of outer wheel, leftRight is 2, car goes left
                        else if (x - originalCenterX < -25) {
                            leftRight = 2;
                        }

                        else {
                            leftRight = 0;
                        }

                        // If control wheel is above center position, forwardBackward is 0, car moves forward
                        if (y - padding - originalCenterY < 0) {
                            forwardBackward = 0;
                        }

                        // If control wheel is below center position, forwardBackward is 1, car moves backward
                        else if (y - padding - originalCenterY > 0){
                            forwardBackward = 1;
                        }

                        // If control wheel is exactly at center position, forwardBackward is -1, car stops
                        else {
                            forwardBackward = -1;
                        }



                        // Sends concatenated command to Arduino in the form of leftRight forwardBackward Speed,, the ">" is the end message marker
                        if (myBluetoothConnection.isConnected() == true){
                            if (forwardBackward == -1){
                                myBluetoothConnection.send("99999>");
                            }
                            else {
                                myBluetoothConnection.send(String.valueOf(leftRight)+String.valueOf(forwardBackward)+speedString+">");
                            }
                        }
                    }
                    break;

                // Stop touching screen
                case MotionEvent.ACTION_UP:
                    synchronized (this) {

                        // Move inner circle back to original position
                        controlWheelInner.setX(originalX);
                        controlWheelInner.setY(originalY);

                        // Send stop signal
                        if (myBluetoothConnection.isConnected() == true)
                        {
                            myBluetoothConnection.send("99999>");
                        }
                    }
                    break;
            }
            return true;
        }
    };
}
