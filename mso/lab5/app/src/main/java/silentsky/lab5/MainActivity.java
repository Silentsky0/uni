package silentsky.lab5;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.Arrays;

import silentsky.lab5.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'lab5' library on application startup.
    static {
        System.loadLibrary("lab5");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        int[] data = new int[]{10, 10, 10, 10, 1, 1, 1, 2, 2, 2, 3, 3, 3, 5, 6, 7, 7, 7, 9, 9};
        int[] result = removeDuplicates(data);

        TextView originalArrayContents = findViewById(R.id.originalArrayContents);
        TextView noDuplicatesArrayContents = findViewById(R.id.arrayNoDuplicatesContents);

        originalArrayContents.setText(Arrays.toString(data));
        noDuplicatesArrayContents.setText(Arrays.toString(result));
    }

    /**
     * A native method that is implemented by the 'lab5' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int[] removeDuplicates(int[] array);
}