package org.silentsky;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

public class SecondActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);

        final TextView textView = findViewById(R.id.textViewResult);

        if (getIntent().hasExtra("Request")) {
            textView.setText(getIntent().getStringExtra("Request"));
        }
    }

    @Override
    public void onBackPressed() {

        AlertDialog.Builder adb = new AlertDialog.Builder(this);
        adb.setTitle("Czy chcesz zakończyć?");
        adb.setPositiveButton("Zakończ",
                (dialog, which) -> {
                    Intent resultIntent = new Intent();
                    resultIntent.putExtra("Result", "Wynik!");
                    setResult(RESULT_OK, resultIntent);
                    finish();
                });
        adb.setNegativeButton("Nie",
                (dialog, which) -> {
                });
        adb.create().show();
    }
}