
package com.thegamecreators.agk_player;

import android.util.Log;

import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.FirebaseInstanceIdService;

public class FCMService extends FirebaseInstanceIdService {

    @Override
    public void onTokenRefresh()
    {
        AGKHelper.GCM_PNRegID = FirebaseInstanceId.getInstance().getToken();
        Log.e( "Push Token", ": " + AGKHelper.GCM_PNRegID );
    }
}
