package com.example.androidex;

import android.app.Service;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

public class MusicService extends Service {
	public static boolean isStop = false;
    private IBinder mIBinder = new MyBinder();
    MediaPlayer mp3_door_opening, mp3_door_closing;
    public enum MusicType{
    	DOOR_OPENING, DOOR_CLOSING
    }
    
    class MyBinder extends Binder{
        MusicService getService(){
            return MusicService.this;
        }
    }
    @Override
    public IBinder onBind(Intent intent) {
        Log.e("LOG", "onBind()");
        return mIBinder;
    }

    @Override
    public void onCreate() {
        Log.e("LOG", "onCreate()");
        super.onCreate();
        mp3_door_opening = MediaPlayer.create(this,R.raw.door_opening);
        mp3_door_closing = MediaPlayer.create(this,R.raw.door_closing);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.e("LOG", "onStartCommand()");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        Log.e("LOG", "onDestroy()");
        super.onDestroy();
    }

    //service function : return count
    public void PlayMusic(MusicType type){
    	switch(type){
    	case DOOR_OPENING:
    		mp3_door_opening.start();
    		break;
    	case DOOR_CLOSING:
    		mp3_door_closing.start();
    		break;
    	}
    }
    
    @Override
    public boolean onUnbind(Intent intent) {
        Log.e("LOG", "onUnbind()");
        isStop = true;
        return super.onUnbind(intent);
    }
}

