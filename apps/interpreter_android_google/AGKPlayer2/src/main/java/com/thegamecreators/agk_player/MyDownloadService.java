package com.thegamecreators.agk_player;

import android.os.Messenger;
import android.util.Log;

import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.impl.DownloaderService;

public class MyDownloadService extends DownloaderService {
    // You must use the public key belonging to your publisher account
    public static String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqV2cB4Ih8C8mPqzCDrAxe4gatHx6JNx/lC9H3hIhEmnDljeWkQBLqiRFTG4ocOv1YjSMLrxvK3LYaSlGuEzTk+KxHAu3tTJzgtQ1ZcajbeqT8f1Xc+3p6+qMccPrl0DbWAIrRku2f5q0f5XoAkJHGMXmRFjZ56puwW1MYEjy40i7HulWFhj3HmfedopXE06jwjhzZxUda9qCAl0aRUq/TJ6mX6BBVAloBrPNNWitCIj/x18P5ItWpwAVGb3MbZnSFR7BKzmGIUnGYG1ULISWC8evf/zDD0N3lWMUChzd1kPr6/Ok8mfjuMVYd4L+mx5KC/sBazJb7v4h8DNLNgy7qQIDAQAB";
    // You should also modify this salt
    public static final byte[] SALT = new byte[] { 1, 42, -12, -1, 54, 98,
            -100, -12, 43, 2, -8, -4, 9, 5, -106, -107, -33, 45, -1, 84
    };

    @Override
    public String getPublicKey() {
        return BASE64_PUBLIC_KEY;
    }

    @Override
    public byte[] getSALT() {
        return SALT;
    }

    @Override
    public String getAlarmReceiverClassName() {
        return MyAlarmReceiver.class.getName();
    }
};

class MyDownloadClient implements IDownloaderClient
{
	private IDownloaderService mRemoteService;
	
	@Override
	public void onServiceConnected(Messenger m) {
		Log.e("Download Expansion Client","Service Connected");
	    mRemoteService = DownloaderServiceMarshaller.CreateProxy(m);
	    mRemoteService.onClientUpdated(AGKHelper.g_pDownloaderStub.getMessenger());
	}
	
	@Override
	public void onDownloadStateChanged(int newState)
	{
		switch( newState )
		{
			case STATE_COMPLETED: 
			{
				Log.e("Download Expansion","Finished");
				AGKHelper.g_iExpansionState = 3; 
				break;
			}
			case STATE_FAILED_UNLICENSED: 
			{
				Log.e("Download Expansion","Unlicensed");
				AGKHelper.g_iExpansionState = -1;
				break;
			}
			case STATE_FAILED_FETCHING_URL:
			{
				Log.e("Download Expansion","Failed to fetch URL");
				AGKHelper.g_iExpansionState = -1;
				break;
			}
			case STATE_FAILED_SDCARD_FULL:
			{
				Log.e("Download Expansion","SDCard full");
				AGKHelper.g_iExpansionState = -1;
				break;
			}
			case STATE_FAILED_CANCELED:
			{
				Log.e("Download Expansion","Cancelled");
				AGKHelper.g_iExpansionState = -1;
				break;
			}
			case STATE_FAILED:
			{
				Log.e("Download Expansion","Failed");
				AGKHelper.g_iExpansionState = -1;
				break;
			}
		}
	}
	
	@Override
	public void onDownloadProgress(DownloadProgressInfo progress)
	{
		AGKHelper.g_fExpansionProgress = (progress.mOverallProgress / (float) progress.mOverallTotal) * 100;
	}
}