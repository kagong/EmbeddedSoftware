package com.example.androidex;

import com.example.androidex.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;



public class MainActivity extends Activity{
	
	LinearLayout linear;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		linear = (LinearLayout)findViewById(R.id.container);
		
		Button btn=(Button)findViewById(R.id.newactivity);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				Intent intent=new Intent(MainActivity.this, MainActivity2.class);//call the activity 2 (puzzle game)
				startActivity(intent);
			}
		};
		btn.setOnClickListener(listener);
	}

}
