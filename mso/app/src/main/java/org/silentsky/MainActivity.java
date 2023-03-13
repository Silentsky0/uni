package org.silentsky;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private ImageView imageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button buttonSecondActivity = findViewById(R.id.mybutton);
        final Button buttonShowImage = findViewById(R.id.button_show_image);

        final TextView secondActivityResult = findViewById(R.id.second_activity_result);
        imageView = findViewById(R.id.view_image);

        secondActivityResult.setVisibility(View.INVISIBLE);


        Intent secondActivityIntent = new Intent(this, SecondActivity.class);
        buttonSecondActivity.setOnClickListener(view -> {

            secondActivityIntent.putExtra("Request", "witam");

            startActivityForResult(secondActivityIntent, 999);
        });


        buttonShowImage.setOnClickListener(view -> {
            Intent showImageIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
            startActivityForResult(showImageIntent, 616);
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 999 && resultCode == RESULT_OK) {
                Log.d("ZX", "Result is OK: " + data.getStringExtra("Result"));
                final TextView secondActivityResult = findViewById(R.id.second_activity_result);
                secondActivityResult.setText("Wynik drugiej activity: " + data.getStringExtra("Result"));
                secondActivityResult.setVisibility(View.VISIBLE);
        }
        if (requestCode == 616 && resultCode == RESULT_OK) {
            Bundle extras = data.getExtras();
            Bitmap imageBitmap = (Bitmap) extras.get("data");

            imageView.setImageBitmap(imageBitmap);
        }
    }
}