package com.example.connormurray.cameratracker;

// libraries
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;
import java.io.IOException;
import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640,480,Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;

    private SeekBar rowControl;
    private TextView rowTextView;
    int rowRead = 15;

    private SeekBar colorControl;
    private TextView colorTextView;
    int colorThresh = 600;

    static long prevtime = 0; // for FPS calculation

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
        mSurfaceHolder = mSurfaceView.getHolder();

        mTextureView = (TextureView) findViewById(R.id.textureview);
        mTextureView.setSurfaceTextureListener(this);

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        rowControl = (SeekBar) findViewById(R.id.rowSeek);
        rowTextView = (TextView) findViewById(R.id.rowView);

        colorControl = (SeekBar) findViewById(R.id.colorSeek);
        colorTextView = (TextView) findViewById(R.id.colorView);

        paint1.setColor(0xffff0000); // red
        paint1.setTextSize(24);

        setMyControlListener();
    }

    private void setMyControlListener(){
        rowControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                rowRead = progress;

                rowTextView.setText("Row: " +progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        colorControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                colorThresh = progress;

                colorTextView.setText("Color: " +progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setColorEffect(Camera.Parameters.EFFECT_MONO); // black and white
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {

            int sep = 100;

            int[] pixelsTop = new int[bmp.getWidth()];
            int[] pixelsMid = new int[bmp.getWidth()];
            int[] pixelsBot = new int[bmp.getWidth()];

            int startY = rowRead; // which row in the bitmap to analyse to read
            // only look at one row in the image
            bmp.getPixels(pixelsTop, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)
            bmp.getPixels(pixelsMid, 0, bmp.getWidth(), 0, startY+sep, bmp.getWidth(), 1);
            bmp.getPixels(pixelsBot, 0, bmp.getWidth(), 0, startY+2*sep, bmp.getWidth(), 1);

            // pixels[] is the RGBA data (in black an white).
            // instead of doing center of mass on it, decide if each pixel is dark enough to consider black or white
            // then do a center of mass on the thresholded array

            //top
            int[] thresholdedPixelsTop = new int[bmp.getWidth()];
            int wbTotalTop = 0; // total mass
            int wbCOMTop = 0; // total (mass time position)
            for (int i = 0; i < bmp.getWidth(); i++) {
                // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                // if it is greater than some value (600 here), consider it black
                // play with the 600 value if you are having issues reliably seeing the line
                if (255*3-(red(pixelsTop[i])+green(pixelsTop[i])+blue(pixelsTop[i])) > colorThresh) {
                    thresholdedPixelsTop[i] = 255*3;
                }
                else {
                    thresholdedPixelsTop[i] = 0;
                }
                wbTotalTop = wbTotalTop + thresholdedPixelsTop[i];
                wbCOMTop = wbCOMTop + thresholdedPixelsTop[i]*i;
            }
            int COMTop;
            //watch out for divide by 0
            if (wbTotalTop<=0) {
                COMTop = bmp.getWidth()/2;
            }
            else {
                COMTop = wbCOMTop/wbTotalTop;
            }

            // draw a circle where you think the COM is
            canvas.drawCircle(COMTop, startY, 5, paint1);

            // also write the value as text
            canvas.drawText("COM = " + COMTop, 10, startY, paint1);


            //middle
            int[] thresholdedPixelsMid = new int[bmp.getWidth()];
            int wbTotalMid = 0; // total mass
            int wbCOMMid = 0; // total (mass time position)
            for (int i = 0; i < bmp.getWidth(); i++) {
                // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                // if it is greater than some value (600 here), consider it black
                // play with the 600 value if you are having issues reliably seeing the line
                if (255*3-(red(pixelsMid[i])+green(pixelsMid[i])+blue(pixelsMid[i])) > colorThresh) {
                    thresholdedPixelsMid[i] = 255*3;
                }
                else {
                    thresholdedPixelsMid[i] = 0;
                }
                wbTotalMid = wbTotalMid + thresholdedPixelsMid[i];
                wbCOMMid = wbCOMMid + thresholdedPixelsMid[i]*i;
            }
            int COMMid;
            //watch out for divide by 0
            if (wbTotalMid<=0) {
                COMMid = bmp.getWidth()/2;
            }
            else {
                COMMid = wbCOMMid/wbTotalMid;
            }

            // draw a circle where you think the COM is
            canvas.drawCircle(COMMid, startY+sep, 5, paint1);

            // also write the value as text
            canvas.drawText("COM = " + COMMid, 10, startY+sep, paint1);


            //bottom
            int[] thresholdedPixelsBot = new int[bmp.getWidth()];
            int wbTotalBot = 0; // total mass
            int wbCOMBot = 0; // total (mass time position)
            for (int i = 0; i < bmp.getWidth(); i++) {
                // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                // if it is greater than some value (600 here), consider it black
                // play with the 600 value if you are having issues reliably seeing the line
                if (255*3-(red(pixelsBot[i])+green(pixelsBot[i])+blue(pixelsBot[i])) > colorThresh) {
                    thresholdedPixelsBot[i] = 255*3;
                }
                else {
                    thresholdedPixelsBot[i] = 0;
                }
                wbTotalBot = wbTotalBot + thresholdedPixelsBot[i];
                wbCOMBot = wbCOMBot + thresholdedPixelsBot[i]*i;
            }
            int COMBot;
            //watch out for divide by 0
            if (wbTotalBot<=0) {
                COMBot = bmp.getWidth()/2;
            }
            else {
                COMBot = wbCOMBot/wbTotalBot;
            }

            // draw a circle where you think the COM is
            canvas.drawCircle(COMBot, startY+2*sep, 5, paint1);

            // also write the value as text
            canvas.drawText("COM = " + COMBot, 10, startY+2*sep, paint1);



            c.drawBitmap(bmp, 0, 0, null);
            mSurfaceHolder.unlockCanvasAndPost(c);

            // calculate the FPS to see how fast the code is running
            long nowtime = System.currentTimeMillis();
            long diff = nowtime - prevtime;
            mTextView.setText("FPS " + 1000/diff);
            prevtime = nowtime;
        }
    }
}