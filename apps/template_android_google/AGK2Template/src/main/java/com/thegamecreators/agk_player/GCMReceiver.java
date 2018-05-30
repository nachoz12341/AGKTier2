package com.thegamecreators.agk_player;

import android.content.Context;
import com.google.android.gcm.GCMBroadcastReceiver;

public class GCMReceiver extends GCMBroadcastReceiver { 
    @Override
	protected String getGCMIntentServiceClassName(Context context) { 
		return "com.thegamecreators.agk_player.GCMIntentService"; 
	} 
}