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
    /*
    private IMyCounterService binder;
    private ServiceVonnection connection = new ServiceConnection(){
       // @override
        public boid onServiceConnection(ComponentName name, IBinder service){
         //   binder = IMyCounterService.Stub.asInterface(service);
        //}
    }*/
	private void initButtons(){
		buttonLinear.removeAllViews();
	}
	OnClickListener button_listener=new OnClickListener(){
		public void onClick(View v){
            push_button(Integer.parseInt(v.getText().toString()));
            if(check_button() == true){
                initButtons();
                stopTimer();
            }
		}
	};
    private void startTimer(){
        Intent intent = new Intent(MainActivity2.this, MyCounterService.class)
            bindService(intent,connection,BIND_AUTO_CREATE)

    }
    private void stopTimer(){
    }
    private bool check_button(){
        for(int i=0;i<row*col-1;i++){
            if(!btns[i].getText().toString().equals(Integer.toString(i)))
                return false;
        }
        if(btns[row*col-1].getText().toString().equals("0"))
            return true;
        return false;
    }
    private void swap_button(int idx,int target){
        btns[target].setBackgroundResource(android.R.drawable.btn_default);
        btns[target].setText(Integer.toString(idx));
        btns[idx].setBackgroundColor(Color.BLACK);
        btns[idx].setText("0");
    }
    private void push_button(int idx){
        if(idx == 0)
            return ;
        for(int i=0;i<row*col;i++){
            if(btns[i].getText().toString().equals(Integer.toString(idx))){
                int r = i /col,c = i % col;
                if(r+1 < row){
                    int target= idx + col;
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,target);
                        break;
                    }
        		}
                if(r-1 >= 0){
                    int target = idx -col; 
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,target);
                        break;
                    }
                }
                if(c+1 < col){
                    int target = idx + 1; 
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,target);
                        break;
                    }
                }
                if (c-1 >=0){
                    int target = idx - 1; 
                    if(btns[target].getText().toString().equals("0")){
                        swap_button(idx,target);
                        break;
                    }
                }
            }
        }
    }
    private void makepuzzle(){
        Random rnd = new Random();
        for(int i=0;i<20;i++){
            int idx = rnd.nextInt(row*col-1);
            push_button(idx);
        }
    }
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
        btns[row*col -1].setText("0");
        btns[row*col -1].setBackgroundColor(Color.BLACK);
        make_puzzle();
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
                startTimer():
			}
		};
		makeBtn.setOnClickListener(make_listener);
	
		
	}

}
