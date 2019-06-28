package com.example.androidex;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;


public class MainActivity2 extends Activity{
	public MusicService mService;

	int[][] customer = new int[8][10];
	int elevator_pos;
	int dest_elevator;
	
	int ndk_open_flag = 1;
	int open_flag = 0;
	//Member Variable
	//List<Person> people;
	//StateUpDown buttonState; NONE(0),UP(1),DOWN(2),UPDOWN(3);	
	//int floorNum;
	List<EVSystem.Floor> floors;
	//ArrayList<EVSystem.Floor> Message_floors;
	int movetic;

	int personNum;
	//Member Variable
	//int personNum;
	//int nowFloor;
	//boolean btnstate[] = new boolean[7];
	//StateMove stateMove; MOVE(0),STOP(1);
	//StateUpDown stateUpDown; NONE(0),UP(1),DOWN(2),UPDOWN(3);	
	//StateStop stateStop; OPEN(0),IDLE(1);
	EVSystem.Elevator Message_elevator;

	final int[] floor_margin = {0, 434, 365 ,292, 219, 146, 73, 0};

	private boolean elevator_going_flag;

	ServiceConnection sconn = new ServiceConnection(){
		@Override //Call when service start
		public void onServiceConnected(ComponentName name, IBinder service) {
			MusicService.MyBinder myBinder = (MusicService.MyBinder) service;
			mService = myBinder.getService();
			Log.e("LOG", "onServiceConnected()");
		}

		@Override //Call when Service end
		public void onServiceDisconnected(ComponentName name) {
			mService = null;
			Log.e("LOG", "onServiceDisconnected()");
		}

	};


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		//Start Timer Service
		Intent intent = new Intent(MainActivity2.this,MusicService.class);
		bindService(intent, sconn,Context.BIND_AUTO_CREATE);


		//       test_start();

		ndk_open_flag =0;
		open_flag = 1;
		
		System.loadLibrary("ndk-exam");
		new Thread(new EVSystem()).start();
		
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		for (int i = 0 ; i < 7 ; i++){
			for (int j = 0 ; j < 10 ; j++){
				customer[i][j] = 0;
			}
		}
		
		new Thread(new get_data()).start();
	}


	private class get_data implements Runnable{
		TextView Tv = (TextView)findViewById(R.id.elevator_tv);
		

		int get_elv_pos;
		int state;

		@Override
		public void run(){
			int[] prev_length = new int[10];
			while(true){
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				int flag = 1;
				

				for (int i = 0 ; i < 7 ; i++){
					if (prev_length[i] != floors.get(i).people.size()){
		/*				for (int j = 0 ; j < floors.get(i).people.size() ; j++){
							if (floors.get(i).people.get(j).state.getValue() != customer[i][j]){
								flag = 0;
							}
						}
					}
					else{*/
						flag = 0;
						prev_length[i] = floors.get(i).people.size();				
					}
					
					//Log.d("size", "" + prev_length[i] + " and " + floors.get(i).people.size());
				}				
				//Log.d("pass", "next");
				
				
				if (flag == 0){
					for (int i = 0 ; i < 7 ; i++){
						for (int j = 0 ; j < 10 ; j++){
							if (j < floors.get(i).people.size())
								customer[i][j] = floors.get(i).people.get(j).state.getValue();
							else{
								customer[i][j] = 0;
							}
						}
					}
					print_customer();
				}


				int get_movtic = (600 - movetic) * 434 / 600;

				final int finalI = get_movtic;
				final int finalP = personNum;
				runOnUiThread(new Runnable(){
					@Override
					public void run(){
						
						LinearLayout.LayoutParams mLayoutParams =
								(LinearLayout.LayoutParams) Tv.getLayoutParams();
						mLayoutParams.topMargin = finalI;
						Tv.setLayoutParams(mLayoutParams);
						Tv.setText("Number of Person : "+Integer.toString(finalP));
					}
				});


				if (ndk_open_flag == 1 && open_flag == 1){
					mService.PlayMusic(MusicService.MusicType.DOOR_OPENING);
					open_flag = 0;
				}
				else if (ndk_open_flag == 0 && open_flag == 0){
					mService.PlayMusic(MusicService.MusicType.DOOR_CLOSING);
					open_flag = 1;
				}
				
			}
		}
	}



	protected void test_start(){



		for(int i = 0 ; i < 8 ; i++){
			for (int j = 0 ; j < 10 ; j++){
				customer[i][j] = 0;
			}
		}

		customer[6][4] = 1;
		customer[1][1] = -1;
		customer[1][2] = 1;

		elevator_pos = 1;
		dest_elevator = 5;

		print_customer();			
		//    move_elevator();	

	}
	protected void print_customer(){
		
		final LinearLayout[] linear = new LinearLayout[8];

		linear[0] = (LinearLayout)findViewById(R.id.floor1);
		linear[1] = (LinearLayout)findViewById(R.id.floor2);
		linear[2] = (LinearLayout)findViewById(R.id.floor3);
		linear[3] = (LinearLayout)findViewById(R.id.floor4);
		linear[4] = (LinearLayout)findViewById(R.id.floor5);
		linear[5] = (LinearLayout)findViewById(R.id.floor6);
		linear[6] = (LinearLayout)findViewById(R.id.floor7);
		

		Log.d("print", "iunin");


		for(int i = 0 ; i < 7 ; i++){
			final int ii = i;

			runOnUiThread(new Runnable(){
				@Override
				public void run(){
					linear[ii].removeAllViews();
				}
			});		
		}


		for (int i = 0 ; i < 7 ; i++){
			for(int j = 0 ; j < 10 ; j++){
				if(customer[i][j] == 0) continue;
				

				final ImageView img = new ImageView(this);
				
				final int ii = i;
				final int jj = j;	
				runOnUiThread(new Runnable(){

					@Override
					public void run(){
						LayoutParams param = new LayoutParams(80, 55);
						img.setLayoutParams(param);
						if (customer[ii][jj] == 1){
							img.setImageResource(R.drawable.going_up);
						}
						else if (customer[ii][jj] == 2){
							img.setImageResource(R.drawable.going_down);
						}
						linear[ii].addView(img);	

					}
				});
				
				
			}
		}		



	}

	enum StateUpDown{
		NONE(0),UP(1),DOWN(2),UPDOWN(3);	
		private final int value;
		private StateUpDown(int value){
			this.value = value;
		}
		public int getValue(){
			return this.value;
		}
		public boolean isContain(StateUpDown target) {
			if(this.value == 3 && target.getValue() >= 1)
				return true;
			return false;
		}
		// updown -> up (temp = down),updown -> down (temp = up)
		// up -> NONE (temp = up)
		public static StateUpDown getState(int val){
			switch(val){
			case 0:
				return StateUpDown.NONE;
			case 1:
				return StateUpDown.UP;
			case 2:
				return StateUpDown.DOWN;
			case 3:
				return StateUpDown.UPDOWN;
			}
			return StateUpDown.NONE;
		}
	}
	enum StateMove{
		MOVE(0),STOP(1);
		private final int value;
		private StateMove(int value){
			this.value = value;
		}
		public int getValue(){
			return value;
		}
		public StateMove changeState() {
			return (this.value == 0)? StateMove.STOP: StateMove.MOVE;
		}

	}
	enum StateStop{
		OPEN(0),IDLE(1);
		private final int value;
		private StateStop(int value){
			this.value = value;
		}
		public int getValue(){
			return value;
		}
	}

	enum IntrBtn{
		VOLUP(0),VOLDOWN(1),BACK(2);
		private final int value;
		private IntrBtn(int value){
			this.value = value;
		}
		public int getValue(){
			return this.value;
		}	

	}
	public class EVSystem implements Runnable{


		public native int getSwitch(int dev_id);
		public native int getIntrBtn(int dev_id);
		public native void setDot(int dev_id,int flag);
		public native void setLed(int dev_id,int data);
		public native void setBuzzer(int dev_id);

		public native int callSyscall(int data1,int data2,int[] data3,int[] data4);
		public native int openDevicea();
		public native int openDeviceb();
		public native void closeDevice(int dev_1,int dev_2);



		public class Floor{

			public class Person{
				private StateUpDown state;
				public Person(StateUpDown state){
					this.state = state;
				}
				public StateUpDown getState(){
					return this.state;
				}
			}
			List<Person> people;
			StateUpDown buttonState;
			int floorNum;

			//constructor for floor class
			public Floor(int floorN){
				this.people = new ArrayList<Person>();
				this.floorNum = floorN;
				this.buttonState = StateUpDown.NONE;
			}
			public void addPerson(StateUpDown state){
				if(state == StateUpDown.UP||state == StateUpDown.DOWN){
					Person person = new Person(state);
					people.add(person);

					if (state == StateUpDown.UP 		&& this.buttonState == StateUpDown.NONE)
						this.buttonState = StateUpDown.UP;
					else if (state == StateUpDown.UP 	&& this.buttonState == StateUpDown.DOWN)
						this.buttonState = StateUpDown.UPDOWN;
					else if (state == StateUpDown.DOWN 	&& this.buttonState == StateUpDown.NONE)
						this.buttonState = StateUpDown.DOWN;
					else if (state == StateUpDown.DOWN 	&& this.buttonState == StateUpDown.UP)
						this.buttonState = StateUpDown.UPDOWN;
				}
				else{
					//error
				}
			}
			public List<Person> getPeople(){
				return this.people;
			}
		}
		public class Elevator{
			int nowFloor;
			boolean btnstate[] = new boolean[7];
			StateMove stateMove;
			StateUpDown stateUpDown;
			StateStop stateStop;
			public Elevator(){
				personNum = 0;
				this.nowFloor = 1;
				for(int i=0;i<7;i++)
					btnstate[i] = false;
				this.stateMove = StateMove.STOP;
				this.stateUpDown = StateUpDown.NONE;
				this.stateStop = StateStop.IDLE;
			}
			public boolean addPerson() {
				if(personNum + 1 > 8) { //max number of person in elevator
					return false;
				}
				personNum += 1;
				return true;
			}
			public void evitPerson(){
				if(this.stateMove == StateMove.STOP && personNum > 0){
					personNum-=1;
				}
			}
		}
		Elevator elevator;
		int target_floor;
		boolean running,openREQ;

		//constructor for elevator system class 
		public EVSystem(){

			floors = new ArrayList<Floor>();
			for(int i = 0 ; i  < 7; i++){
				Floor floor = new Floor(i+1);
				floors.add(floor);
			}
			this.elevator = new Elevator();
			this.target_floor = -1;

			this.openREQ = true;
		}
		public Floor getFloor(int i){//0�겫占쏙옙苑� 占쎈뻻占쎌삂
			return floors.get(i);
		}
		public void calculTarget(){            
			int data1 = this.elevator.nowFloor;//start 1
			int data2 = this.elevator.stateUpDown.getValue();
			int data3[] = new int[8];
			int data4[] = new int[8];
			for(int i= 1 ; i < 8 ;i++) {
				data3[i] = floors.get(i-1).buttonState.getValue();
				data4[i] = (this.elevator.btnstate[i-1] == true)? 1: 0;
			}
			System.out.println(String.format("F %d %d %d %d %d %d %d\n", data3[1],data3[2],data3[3],data3[4],data3[5],data3[6],data3[7]));
			System.out.println(String.format("E %d %d %d %d %d %d %d\n", data4[1],data4[2],data4[3],data4[4],data4[5],data4[6],data4[7]));
			this.target_floor = callSyscall(data1,data2,data3,data4);
			if(this.target_floor > this.elevator.nowFloor)
				this.elevator.stateUpDown = StateUpDown.UP;
			else if(this.target_floor < this.elevator.nowFloor)
				this.elevator.stateUpDown = StateUpDown.DOWN;
			else{
				this.elevator.stateMove = StateMove.STOP;
				this.elevator.stateStop = StateStop.IDLE;
				this.elevator.stateUpDown = StateUpDown.NONE;
			}
		}
		public boolean isIdle(){
			int now = this.elevator.nowFloor;
			for(int i = 0 ; i < 7 ; i++){
				if(i+1 == now )
					continue;
				if(floors.get(i).buttonState != StateUpDown.NONE ||
						this.elevator.btnstate[i] == true)
					return false;
			}
			return true;
		}
		public boolean isOpenning(){
			if(this.openREQ || 
					floors.get(this.elevator.nowFloor - 1).buttonState != StateUpDown.NONE ||
					this.elevator.btnstate[this.elevator.nowFloor -1] == true){
				this.openREQ = false;
				return true;
			}
			return false;
		}

		@Override
		public void run(){
			int stoptic=0,data;
			Random rnd = new Random();
			this.running = true;
			boolean DEBUG = false;
			int dev_1,dev_2;
			int n,btn;
			dev_1 = openDevicea();
			dev_2 = openDeviceb();

			while(this.running){
				n = -1;
				btn = -1;
				if(this.elevator.stateMove == StateMove.STOP) {
					//System.out.println("stop");
					if(this.elevator.stateStop == StateStop.OPEN) {
						stoptic = (stoptic + 2 > 80)? 80 : stoptic + 2;
						if(stoptic >= 80) {//closing
							
							this.elevator.stateMove = StateMove.MOVE;
							this.calculTarget();
							ndk_open_flag = 0;
							//mService.PlayMusic(MusicService.MusicType.DOOR_CLOSING);
							//closing voice
						}
						else{//in open
							Floor temp = floors.get(this.elevator.nowFloor-1);
							boolean isNotFull = true;
							for(int i=0 ; i < temp.people.size();++i) {
								if(this.elevator.nowFloor == 1 || this.elevator.nowFloor == 7 || this.elevator.stateUpDown == StateUpDown.NONE ||temp.people.get(i).state == this.elevator.stateUpDown) {
									if(this.elevator.addPerson()) {
										isNotFull = true;
										temp.people.remove(i); 
                                        i = 0;
									}
									else{
										isNotFull = false;	
									}
								}
							}
							if(isNotFull){
								
								int result = 0;
								for(int i = 0 ; i <temp.people.size();i++){
									result |= temp.people.get(i).state.getValue();
								}
								temp.buttonState = StateUpDown.getState(result);
							}
                            else
							    setBuzzer(dev_1);//1sec ring
						}
					}
					else if(this.isOpenning() == true){
						ndk_open_flag = 1;
						//mService.PlayMusic(MusicService.MusicType.DOOR_OPENING);
						
							System.out.println("opennig");
						stoptic = 0;
						this.elevator.stateStop = StateStop.OPEN;
						this.elevator.btnstate[this.elevator.nowFloor-1] = false;
						floors.get(this.elevator.nowFloor-1).buttonState = StateUpDown.NONE;
                        
					}
					else{

						if(this.isIdle() == false){
							this.elevator.stateMove = StateMove.MOVE;
					        this.calculTarget();
						}

					}
				}
				else {//move 10sec
					if(this.elevator.stateUpDown == StateUpDown.UP) 
						movetic = (movetic + 2 > 600) ? 600 : movetic + 2;
					else if(this.elevator.stateUpDown == StateUpDown.DOWN) 
						movetic = (movetic - 2 < 0) ? 0 : movetic - 2;

					if(movetic % 100 == 0) {//arrive floor
						
						if(DEBUG == true)
							System.out.println("floorarrive");
						int num = movetic /100;
						//destination of elev or there is passenger in arrive floor
						this.elevator.nowFloor = num + 1;

						System.out.println(String.format("now : %d  target: %d \n",num+1,this.target_floor));
						if(this.elevator.nowFloor == this.target_floor) {
							if(DEBUG == true)
								System.out.println("stopping");
							this.openREQ = true;
							this.elevator.stateMove = StateMove.STOP;
							this.elevator.stateStop = StateStop.IDLE;
							this.elevator.btnstate[num] = false;
					        this.calculTarget();
					        System.out.println(String.format("now : %d  target: %d \n",num+1,this.target_floor));
						}
					}

				}

				n = getSwitch(dev_2);
				btn = getIntrBtn(dev_1);

				if(n != -1) {
					if(0<= n && n <=6 && personNum > 0){
						this.elevator.btnstate[n] = true;
					    this.calculTarget();
                    }
					else if(n == 7 && this.elevator.stateMove == StateMove.STOP)//open
					    this.openREQ = true;
					else if(n == 8 && this.elevator.stateMove == StateMove.STOP)//close
						stoptic = (stoptic < 70 )? 70 : stoptic;

				} 
				if(btn != -1){
					if(btn == IntrBtn.VOLUP.getValue()){//down
						int floorNum = rnd.nextInt(6);
						floors.get(floorNum).addPerson(StateUpDown.UP);
					    this.calculTarget();
					}
					else if(btn == IntrBtn.VOLDOWN.getValue()){//back
						int floorNum = rnd.nextInt(6)+1;
						floors.get(floorNum).addPerson(StateUpDown.DOWN);
					    this.calculTarget();
					}
					else if(btn == IntrBtn.BACK.getValue()){//up
						this.elevator.evitPerson();
					}
				}

				//callSyscall data;


				
				if(elevator.stateUpDown == StateUpDown.NONE)
					this.setDot(dev_1,0);
				if(elevator.stateUpDown== StateUpDown.UP)
					this.setDot(dev_1,1);
				if(elevator.stateUpDown == StateUpDown.DOWN)
					this.setDot(dev_1,2);

				data = 0;
				for(int i = 0 ; i < 7 ; i++) 
					data += ((this.elevator.btnstate[i] == true)? 1: 0) << i;
				this.setLed(dev_1,data);

				try { Thread.sleep(100); } catch (InterruptedException e) { e.printStackTrace(); }

			}
			closeDevice(dev_1,dev_2);
		}
		public void stop(){
			this.running = false;
		}

	}

}


