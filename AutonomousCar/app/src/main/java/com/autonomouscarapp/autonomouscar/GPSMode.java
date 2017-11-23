package com.autonomouscarapp.autonomouscar;

import android.os.Bundle;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import android.location.Location;
import android.location.Address;
import android.location.Geocoder;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;
import android.widget.Button;
import android.Manifest;
import android.widget.EditText;
import com.google.android.gms.maps.GoogleMap.OnMyLocationButtonClickListener;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import android.content.DialogInterface;
import android.support.v4.content.ContextCompat;
import android.content.pm.PackageManager;
import android.support.v7.app.AlertDialog;
import java.util.*;

/*
 * An activity that displays a Google map showing the place at the device's current location.
 */
public class GPSMode extends AppCompatActivity
        implements OnMapReadyCallback,
        OnMyLocationButtonClickListener,
        GoogleApiClient.ConnectionCallbacks,
        GoogleApiClient.OnConnectionFailedListener,
        LocationListener {

    GoogleMap mMap;
    LocationRequest mLocationRequest;
    GoogleApiClient mGoogleApiClient;
    // The last-known geological location(current one) retrieved by the Fused Location Provider.
    Location mLastLocation;
    // Marking out the current location
    Marker mLocationMarker;
    Button searchButton;
    Double myLatitude = null;
    Double myLongitude = null;
    EditText searchBarTextField = null;
    LatLng myLocation;
    public static final int PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION = 1;
    public static final int DEFAULT_ZOOM = 18;
    /* Initialize the map activity
    */
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gpsmode);
        getSupportActionBar().setTitle("Map Location Activity");

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.googleMap);
        mapFragment.getMapAsync(this);

        searchBarTextField = (EditText) findViewById(R.id.searchBarTextField);
        searchButton=(Button) findViewById(R.id.searchButton);

        // Defining button click event listener for the LocationSearchButton
        searchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String locationSearched = searchBarTextField.getText().toString();
                if (locationSearched != null && !locationSearched.equals("")) {
                    Geocoder geocoder = new Geocoder(getBaseContext());
                    List<Address> addresses = null;
                    try {
                        // Getting a maximum of 3 Address that matches the input text
                        addresses = geocoder.getFromLocationName(locationSearched, 3);
                        if (addresses != null && !addresses.equals(""))
                            search(addresses);
                    } catch (Exception e) {
                    }
                }
            }
        });
    }
    protected void search(List<Address> addresses) {
        Address address = (Address) addresses.get(0);
        LatLng locationSearched = new LatLng(address.getLatitude(), address.getLongitude());
        String addressText = String.format(
                "%s, %s",
                address.getMaxAddressLineIndex() > 0 ? address
                        .getAddressLine(0) : "", address.getCountryName());
        MarkerOptions markerOptions = new MarkerOptions();
        markerOptions.position(locationSearched);
        markerOptions.title(addressText);
        mMap.clear();
        mMap.addMarker(markerOptions);
        mMap.moveCamera(CameraUpdateFactory.newLatLng(locationSearched)); // Move to location searched
        mMap.animateCamera(CameraUpdateFactory.zoomTo(15)); // Zoom in
    }

    @Override
    public void onPause() {
        super.onPause();
        // Stop updating locations when Activity is no longer active
        if (mGoogleApiClient != null) {
            LocationServices.FusedLocationApi.removeLocationUpdates(mGoogleApiClient, this);
        }
    }

    // Manipulates the map once it's available;
    @Override
    public void onMapReady(GoogleMap googleMap) {

        mMap = googleMap;
        mMap.setMapType(GoogleMap.MAP_TYPE_NORMAL);

        //Initialize Google Play Services
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                == PackageManager.PERMISSION_GRANTED) {//Location Permission granted
            buildGoogleApiClient();
            mMap.setMyLocationEnabled(true);
        } else {
            if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
                //Request Location Permission
                requestLocationPermission();
        }

        mMap.setOnMyLocationButtonClickListener(this);
    }

    protected synchronized void buildGoogleApiClient() {
        mGoogleApiClient = new GoogleApiClient.Builder(this)
                .addApi(LocationServices.API)
                .addConnectionCallbacks(this)
                .addOnConnectionFailedListener(this)
                .build();
        mGoogleApiClient.connect();
    }

    // Accesses location of object
    @Override
    public void onConnected(Bundle bundle) {
        mLocationRequest = new LocationRequest();
        mLocationRequest.setInterval(1000);
        mLocationRequest.setFastestInterval(1000);
        mLocationRequest.setPriority(LocationRequest.PRIORITY_BALANCED_POWER_ACCURACY);
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_FINE_LOCATION)
                == PackageManager.PERMISSION_GRANTED) {
            //Update location
            LocationServices.FusedLocationApi.requestLocationUpdates(mGoogleApiClient, mLocationRequest, this);
        }
    }
    @Override
    public void onConnectionSuspended(int i) {}
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult) {}
    @Override
    public void onLocationChanged(Location location)
    {
        mLastLocation = location; //store current location data
        if (mLocationMarker != null) { //remove the previous location marker
            mLocationMarker.remove();
        }
        myLatitude = location.getLatitude();
        myLongitude = location.getLongitude();

        // Place current location marker
        myLocation = new LatLng(myLatitude, myLongitude);
        MarkerOptions markerOptions = new MarkerOptions();
        markerOptions.position(myLocation);
        markerOptions.title("Current Position");
        markerOptions.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_RED));
        mLocationMarker = mMap.addMarker(markerOptions);

        // Move map camera, animates to my current location with a default zoom
        mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(myLocation,DEFAULT_ZOOM));
    }
    private void requestLocationPermission() {
        if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                Manifest.permission.ACCESS_FINE_LOCATION)) {
            // Show an explanation to the user and request permission again
            new AlertDialog.Builder(this)
                    .setTitle("Location Permission Needed")
                    .setMessage("This app needs the Location permission, please accept to use location functionality")
                    .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            //Prompt the user once explanation has been shown
                            ActivityCompat.requestPermissions(GPSMode.this,
                                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                                    PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION );
                        }
                    })
                    .create()
                    .show();
        } else {// No explanation needed, request the permission.
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                    PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION );
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted
                    if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                            == PackageManager.PERMISSION_GRANTED) {
                        mMap.setMyLocationEnabled(true);
                        if (mGoogleApiClient == null) {
                            buildGoogleApiClient();
                        }
                    }
                } else {
                    // Permission denied
                    Toast.makeText(this, "This app requires location permissions to be granted",
                            Toast.LENGTH_LONG).show();
                }
                return;
            }
        }
    }
    @Override
    public boolean onMyLocationButtonClick() {
        mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(myLocation,DEFAULT_ZOOM));
        // The camera animates to the user's current position).
        return false;
    }
}