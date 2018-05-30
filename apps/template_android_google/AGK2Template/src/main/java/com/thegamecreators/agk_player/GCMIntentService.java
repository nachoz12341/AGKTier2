
package com.thegamecreators.agk_player;

import android.app.NativeActivity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import com.mycompany.mytemplate.R;

import com.google.android.gcm.GCMBaseIntentService;

public class GCMIntentService extends GCMBaseIntentService {

    @Override
    public void onRegistered(Context context, String regId)
    {
        AGKHelper.GCM_PNRegID = regId;
    }
    
    @Override
    public void onUnregistered(Context context, String regId)
    {
    	
    }
    
    @Override
    public void onMessage(Context context, Intent intent)
    {
        if ( AGKHelper.isVisible == 1 ) return;

    	String message = intent.getStringExtra("message");
    	String title = intent.getStringExtra("title");
    	    	
    	NotificationManager notificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE); 

    	Intent intent2 = new Intent(this, AGKActivity.class);
    	PendingIntent pIntent = PendingIntent.getActivity(this, 0, intent2, 0);
    	
    	CharSequence from = title;

        Bitmap largeIcon = null;
        try
        {
            largeIcon = BitmapFactory.decodeResource(context.getResources(), R.drawable.icon);
        }
        catch (Exception ex)
        {
            Log.i("AGK", "Failed to create large icon from app icon");
            ex.printStackTrace();
        }

    	Notification notif = new NotificationCompat.Builder(context)
    	.setContentTitle(from)
    	.setContentText(message)
    	.setSmallIcon(R.drawable.icon_white)
    	.setContentIntent(pIntent)
    	.setAutoCancel(true)
        .setLargeIcon(largeIcon)
        .setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_LIGHTS)
    	.build();
    	
    	notificationManager.notify(0, notif);
    }
    
    @Override
    public void onError(Context context, String errorId)
    {
    	AGKHelper.GCM_PNRegID = "Error";
    }
}
