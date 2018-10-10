package com.thegamecreators.agk_player;

import android.Manifest;
import android.app.Activity;
import android.app.NativeActivity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.media.MediaRecorder;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;

import com.amazon.device.iap.PurchasingService;
import com.thegamecreators.agk_player.iap.AGKPurchasingListener;

public class AGKActivity extends NativeActivity
{
    @Override
    public void onCreate( Bundle state )
    {
        super.onCreate( state );

        Intent intent = getIntent();
        if ( intent != null )
        {
            Uri data = intent.getData();
            if ( data != null ) AGKHelper.g_sLastURI = data.toString();
        }
    }

    @Override
    public void onNewIntent( Intent intent )
    {
        if ( intent == null ) return;
        if ( intent.getData() == null ) return;
        AGKHelper.g_sLastURI = intent.getData().toString();
    }

    @Override
    public void onActivityResult( int requestCode, int resultCode, Intent data )
    {
        if (requestCode == 10002) {
            if (resultCode != Activity.RESULT_OK) {
                Log.i("MediaProjection", "User cancelled");
                return;
            }
            if ( Build.VERSION.SDK_INT >= 21 ) {
                Log.i("MediaProjection", "Starting screen capture");

                int width = AGKHelper.g_pAct.getWindow().getDecorView().getWidth();
                int height = AGKHelper.g_pAct.getWindow().getDecorView().getHeight();
                if ( width > height )
                {
                    if ( width > 1280 ) width = 1280;
                    if ( height > 720 ) height = 720;
                }
                else
                {
                    if ( width > 720 ) width = 720;
                    if ( height > 1280 ) height = 1280;
                }
                int audioSource = 0;
                if ( AGKHelper.g_iScreenRecordMic == 1 ) {
                    if (ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED) {
                        audioSource = MediaRecorder.AudioSource.MIC;
                    }
                    else
                    {
                        Log.w( "Screen Recording", "The app does not have the RECORD_AUDIO permission, video will have no audio" );
                    }
                }

                AGKHelper.mMediaRecorder = new MediaRecorder();
                if ( audioSource > 0 ) AGKHelper.mMediaRecorder.setAudioSource(audioSource);
                AGKHelper.mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.SURFACE);
                AGKHelper.mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
                AGKHelper.mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
                if ( audioSource > 0 )
                {
                    AGKHelper.mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
                    AGKHelper.mMediaRecorder.setAudioEncodingBitRate(96000);
                    AGKHelper.mMediaRecorder.setAudioSamplingRate(44100);
                }
                AGKHelper.mMediaRecorder.setVideoEncodingBitRate(2048 * 1000);
                AGKHelper.mMediaRecorder.setVideoFrameRate(30);
                AGKHelper.mMediaRecorder.setVideoSize(width, height);
                AGKHelper.mMediaRecorder.setOutputFile(AGKHelper.g_sScreenRecordFile);
                try {
                    AGKHelper.mMediaRecorder.prepare();
                }
                catch( Exception e )
                {
                    e.printStackTrace();
                    AGKHelper.mMediaRecorder.release();
                    AGKHelper.mMediaRecorder = null;
                    return;
                }

                DisplayMetrics metrics = AGKHelper.g_pAct.getResources().getDisplayMetrics();
                AGKHelper.mMediaProjection = AGKHelper.mMediaProjectionManager.getMediaProjection(resultCode, data);
                Log.i("MediaProjection", "Setting up a VirtualDisplay: " + width + "x" + height + " (" + metrics.densityDpi + ")");
                AGKHelper.mVirtualDisplay = AGKHelper.mMediaProjection.createVirtualDisplay("ScreenCapture",
                        width, height, metrics.densityDpi,
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
                        AGKHelper.mMediaRecorder.getSurface(), null, null);
                AGKHelper.mMediaRecorder.start();
            }
        }
    }

    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults)
    {
        int index = requestCode-5;
        if ( index < 0 ) return;
        if ( index >= AGKHelper.g_sPermissions.length ) return;

        AGKHelper.g_iPermissionStatus[index] = 2;
    }
}