package com.example.firstgame;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity {

    // Load the native library
    static {
        System.loadLibrary("firstgame");
        Log.d("MainActivity", "Library loaded successfully");
    }

    // Declare native methods
    private native void initGame();
    private native void updateGame();
    private native void startGame();
    private native void pauseGame();  // New method for pausing the game
    private native void resumeGame(); // New method for resuming the game
    private native void stopGame();   // New method for stopping the game

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Set the content view
        setContentView(R.layout.activity_main);  // Make sure your XML layout file is correctly set

        // Initialize the game and update game state
        initGame();
        updateGame();

        Log.d("MainActivity", "onCreate called");

        // Set up the start button and its click listener
        Button startButton = findViewById(R.id.startButton);
        if (startButton != null) {
            startButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    startGame();
                }
            });
        }

        // Set up the pause button and its click listener
        Button pauseButton = findViewById(R.id.pauseButton);
        if (pauseButton != null) {
            pauseButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    pauseGame();
                }
            });
        }

        // Set up the resume button and its click listener
        Button resumeButton = findViewById(R.id.resumeButton);
        if (resumeButton != null) {
            resumeButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    resumeGame();
                }
            });
        }

        // Set up the stop button and its click listener
        Button stopButton = findViewById(R.id.stopButton);
        if (stopButton != null) {
            stopButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    stopGame();
                }
            });
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            hideSystemUi();
        }
    }

    private void hideSystemUi() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }
}
