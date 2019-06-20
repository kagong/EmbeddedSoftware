package com.example.androidex;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;



public class MainActivity2 extends Activity{
	EditText data;
	Button makeBtn;
	LinearLayout linear,buttonLinear;
	int row,col;
	float widthDp,heightDp;
	Button btns[];
	private void initButtons(){
		buttonLinear.removeAllViews();
	}
	OnClickListener button_listener=new OnClickListener(){
		public void onClick(View v){
			if(!((Button)v).getText().toString().equals("0")){
				v.setBackgroundColor(Color.BLACK);

				((Button)v).setText("0");
			}
				
			else{
				((Button)v).setText("1");
				v.setBackgroundResource(android.R.drawable.btn_default);
			}
			
		}
	};
	private void makeButtons(){
		btns = new Button[row*col];
		for(int i= 0 ; i < row; i++){
			LinearLayout test = new LinearLayout(this);
			test.setOrientation(LinearLayout.HORIZONTAL);
			
			for(int j= 0 ; j < col; j++){
				btns[i*col+j] = new Button(this);
				btns[i*col+j].setWidth((int)widthDp/col);
				btns[i*col+j].setHeight((int)heightDp/row);
				btns[i*col+j].setText(Integer.toString(i*col+j+1));
			
				btns[i*col+j].setOnClickListener(button_listener);
				test.addView(btns[i*col+j]);
			}
			buttonLinear.addView(test);
		}
	}
	
		@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main2);
		makeBtn=(Button)findViewById(R.id.makeButton);
		DisplayMetrics dm = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(dm);
		
		widthDp = dm.widthPixels / dm.density;
		heightDp = (dm.heightPixels / dm.density)-160;
		
		buttonLinear=(LinearLayout)findViewById(R.id.buttonContainer);
		
		data=(EditText)findViewById(R.id.textedit);
		data.setText("3 3");
		OnClickListener make_listener=new OnClickListener(){
			public void onClick(View v){
				String temp=data.getText().toString();
				String datas[] = temp.split(" ");
				row = Integer.parseInt(datas[0]);
				col = Integer.parseInt(datas[1]);
				initButtons();
				makeButtons();
			}
		};
		makeBtn.setOnClickListener(make_listener);
	
		
	}

}
