package com.thegamecreators.agk_player;

import android.Manifest;
import android.app.Activity;
import android.app.NativeActivity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.MediaRecorder;
import android.media.projection.MediaProjection;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInStatusCodes;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.games.GamesActivityResultCodes;
import com.google.android.gms.tasks.Task;

public class AGKActivity extends NativeActivity
{
    @Override
    public void onCreate( Bundle state )
    {
        super.onCreate( state );
    }

    @Override
    public void onActivityResult( int requestCode, int resultCode, Intent data )
    {
        if ( requestCode == 9000 || requestCode == 9001 )
        {
            if ( resultCode == Activity.RESULT_OK )
            {
                if ( AGKHelper.m_GameClient != null
                 && !AGKHelper.m_GameClient.isConnected()
                 && !AGKHelper.m_GameClient.isConnecting() )
                {
                    AGKHelper.m_GameClient.connect();
                }
            }
            else
            {
                AGKHelper.ShowMessage( this, "Unable to sign in to Google Play Games" );
            }
        }
        else if ( requestCode == 10001 )
        {
            if (!AGKHelper.mHelper.handleActivityResult(requestCode, resultCode, data))
            {
                Log.e("IAP","Failed to handle activity result "+resultCode);
            }
            else
            {
                Log.i("IAP","Result code handled by IABUtil: "+resultCode);
            }
        }
        else if (requestCode == 10002) {
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

                Log.i( "Screen Recording", "Recording to: " + AGKHelper.g_sScreenRecordFile );

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
        else if ( requestCode == 10003 ) // Google Drive sign in
        {
            Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
            try {
                AGKHelper.g_GoogleAccount = task.getResult(ApiException.class);
                AGKHelper.FinishCloudDataSetup( task );
            }
            catch( ApiException e )
            {
                Log.e( "Google Sign In", "Failed to sign in, error code: " + e.getStatusCode() );
                if ( e.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_CANCELLED ) AGKHelper.g_iCloudDataStatus = -1;
                else if ( e.getStatusCode() != GoogleSignInStatusCodes.SIGN_IN_CURRENTLY_IN_PROGRESS ) AGKHelper.g_iCloudDataStatus = -2;
            }
        }
        else if ( resultCode == GamesActivityResultCodes.RESULT_RECONNECT_REQUIRED )
        {
            Log.i("GameCenter", "Logged out from app");
            AGKHelper.m_GameClient.disconnect();
            AGKHelper.m_GameCenterLoggedIn = 0;
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