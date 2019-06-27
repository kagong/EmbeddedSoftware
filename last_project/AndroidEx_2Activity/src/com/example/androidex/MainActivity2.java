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
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.DisplayMetrics;
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
	
	//Member Variable
	//List<Person> people;
	//StateUpDown buttonState; NONE(0),UP(1),DOWN(2),UPDOWN(3);	
	//int floorNum;
	ArrayList<EVSystem.Floor> Message_floors;
	int Message_movetic;
	
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
        
        
		new Thread(new get_data()).start();
		new Thread(new EVSystem()).start();

	}
		

	private class get_data implements Runnable{
		TextView Tv = (TextView)findViewById(R.id.elevator_tv);

		
		int get_elv_pos;
		int state;
		int open_flag = 1;
		@Override
		public void run(){
			while(true){
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				int flag = 1;
				for (int i = 0 ; i < 8 ; i++){
					for (int j = 0 ; j < 10 ; j++){
						if (Message_floors.get(i).people.get(j).state.getValue() != customer[i][j]){
							flag = 0;
						}
					}
				}				
				if (flag == 0){
					for (int i = 0 ; i < 8 ; i++){
						for (int j = 0 ; j < 10 ; j++){
							customer[i][j] = Message_floors.get(i).people.get(j).state.getValue();
						}
					}
					print_customer();
				}
				
				
				int get_movtic = (600 - Message_movetic) * 434 / 600;
				
				final int finalI = get_movtic;
				runOnUiThread(new Runnable(){
					@Override
					public void run(){
						LinearLayout.LayoutParams mLayoutParams =
								(LinearLayout.LayoutParams) Tv.getLayoutParams();
						mLayoutParams.topMargin = finalI;
						Tv.setLayoutParams(mLayoutParams);
					}
				});
				
				
				if (Message_elevator.stateStop.getValue() == 0 && open_flag == 1){
				    mService.PlayMusic(MusicService.MusicType.DOOR_OPENING);
				    open_flag = 0;
				}
				else if (Message_elevator.stateStop.getValue() == 1 && open_flag == 0){
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
		LinearLayout[] linear = new LinearLayout[8];
		ImageView img;
		
		linear[0] = (LinearLayout)findViewById(R.id.floor1);
		linear[1] = (LinearLayout)findViewById(R.id.floor2);
		linear[2] = (LinearLayout)findViewById(R.id.floor3);
		linear[3] = (LinearLayout)findViewById(R.id.floor4);
		linear[4] = (LinearLayout)findViewById(R.id.floor5);
		linear[5] = (LinearLayout)findViewById(R.id.floor6);
		linear[6] = (LinearLayout)findViewById(R.id.floor7);
		
		for(int i = 0 ; i < 7 ; i++){
			for(int j = 0 ; j < 10 ; j++){
				img = (ImageView)findViewById(i*10+j);
				
				if (img != null){
					img.setVisibility(View.GONE);
				}
			}
		}

		
		for (int i = 0 ; i < 7 ; i++){
			for(int j = 0 ; j < 10 ; j++){
				if(customer[i][j] == 0) continue;
				img = new ImageView(this);
				LayoutParams param = new LayoutParams(80, 55);
				

				img.setLayoutParams(param);
				if (customer[i][j] == 1){
					img.setImageResource(R.drawable.going_up);
				}
				else if (customer[i][j] == 2){
					img.setImageResource(R.drawable.going_down);
				}
				img.setId(i*10 + j);
				linear[i].addView(img);				
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

/*
	    native int getSwitch(int dev_id);
	    native int getIntrBtn(int dev_id);
	    native void setDot(int dev_id,int flag);
	    native void setLed(int dev_id,int data);
	    native void setBuzzer(int dev_id);

	    native int callSyscall(int data1,int data2,int[] data3,int[] data4);
	    native int openDevice_1();
	    native int openDevice_2();
	    native void closeDevice(int dev_1,int dev_2);

	    static {
	      System.loadLibrary("EVSystem");
	    };  
	    */  
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
	        int personNum;
	        int nowFloor;
	        boolean btnstate[] = new boolean[7];
	        StateMove stateMove;
	        StateUpDown stateUpDown;
	        StateStop stateStop;
	        public Elevator(){
	            this.personNum = 0;
	            this.nowFloor = 1;
	            for(int i=0;i<7;i++)
	                btnstate[i] = false;
	            this.stateMove = StateMove.STOP;
	            this.stateUpDown = StateUpDown.NONE;
	            this.stateStop = StateStop.IDLE;
	        }
	        public boolean addPerson() {
	            if(this.personNum + 1 > 8) { //max number of person in elevator
	                this.personNum += 1;
	                return false;
	            }
	            return true;
	        }
	        public void evitPerson(){
	            if(this.stateMove == StateMove.STOP){
	                this.personNum-=1;
	            }
	        }
	    }
	    List<Floor> floors;
	    Elevator elevator;
	    int target_floor;
	    boolean running,openREQ;

	    //constructor for elevator system class 
	    public EVSystem(){
	        floors = new ArrayList<Floor>();
	        for(int i = 0 ; i  < 7; i++){
	            Floor floor = new Floor(i+1);
	            this.floors.add(floor);
	        }
	        this.elevator = new Elevator();
	        this.target_floor = -1;
	        
	        openREQ = false;
	    }
	    public Floor getFloor(int i){//0�겫占쏙옙苑� 占쎈뻻占쎌삂
	        return this.floors.get(i);
	    }
	    public void calculTarget(){      
	    	/*
	        int data1 = this.elevator.nowFloor;//start 1
	        int data2 = this.elevator.stateUpDown.getValue();
	        int data3[] = new int[8];
	        int data4[] = new int[8];
	        for(int i= 1 ; i < 8 ;i++) {
	            data3[i] = this.floors.get(i-1).buttonState.getValue();
	            data4[i] = (this.elevator.btnstate[i-1] == true)? 1: 0;
	        }
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
	        */
	    }
	    public boolean isIdle(){
	        int now = this.elevator.nowFloor;
	        for(int i = 0 ; i+1 != now && i < 7 ; i++){
	            if(this.floors.get(now - 1).buttonState != StateUpDown.NONE ||
	                    this.elevator.btnstate[now -1] == true)
	                return false;
	        }
	        return true;
	    }
	    public boolean isOpenning(){
	        if(this.openREQ || 
	                this.floors.get(this.elevator.nowFloor - 1).buttonState != StateUpDown.NONE ||
	                this.elevator.btnstate[this.elevator.nowFloor -1] == true){
	            openREQ = false;
	            return true;
	        }
	        return false;
	    }
	    @Override
	    public void run(){
	    	this.running = true;
	    	/*
	        int stoptic=0,movetic=0,data;
	        boolean isOpen =false,openningREQ = true;
	        Random rnd = new Random();
	        this.running = true;
	        boolean DEBUG = true;
	        int dev_1,dev_2;
	        */
	       // dev_1 = openDevice_1();
	       // dev_2 = openDevice_2();
	    	int i = 0;
	    	while(running){
	        	
	        	Elevator ev = new Elevator();
	        	ev.personNum= i++;
	        	Message_elevator = ev;
	        	/*
	            int n = -1;
	            int btn = -1;
	            // stop action
	            if(this.elevator.stateMove == StateMove.STOP) {
	                if(DEBUG == true)
	                    System.out.println("stop");
	                if(this.elevator.stateStop == StateStop.OPEN) {
	                    stoptic = (stoptic + 1 > 80)? stoptic : stoptic + 1;
	                    if(stoptic >= 80) {//closing
	                        if(this.isIdle() == true){
	                            this.elevator.stateStop = StateStop.IDLE;
	                            this.elevator.stateUpDown = StateUpDown.NONE;
	                        }
	                        else{
	                            this.elevator.stateMove = StateMove.MOVE;
	                            this.calculTarget();
	                        }
	                        //closing voice
	                    }
	                    else{//in open
	                        Floor temp = this.floors.get(this.elevator.nowFloor-1);
	                        boolean isFull = false;
	                        for(int i=0 ; i < temp.people.size();++i) {
	                            if(this.elevator.nowFloor == 1 || this.elevator.nowFloor == 7 || this.elevator.stateUpDown == StateUpDown.NONE ||temp.people.get(i).state == this.elevator.stateUpDown) {
	                                if(this.elevator.addPerson()) {
	                                    isFull = true;
	                                    temp.people.remove(i); 
	                                }
	                                else{
	                                    isFull = false;	
	                                    setBuzzer(dev_1);//1sec ring
	                                }
	                            }
	                        }
	                        if(isFull)
	                            temp.buttonState = StateUpDown.getState(temp.buttonState.getValue() - this.elevator.stateUpDown.getValue());
	                    }
	                }
	                else if(this.isOpenning() == true){//idle
	                    stoptic = 0;
	                    this.elevator.stateStop = StateStop.OPEN;
	                    this.elevator.btnstate[this.elevator.nowFloor-1] = false;
	                    this.floors.get(this.elevator.nowFloor-1).buttonState = StateUpDown.NONE;//todo
	                }
	            }
	            else {//move 10sec
	                if(DEBUG == true)
	                    System.out.println("move");
	                if(this.elevator.stateUpDown == StateUpDown.UP) 
	                    movetic = (movetic + 1 > 600) ? movetic : movetic + 1;
	                else if(this.elevator.stateUpDown == StateUpDown.DOWN) 
	                    movetic = (movetic - 1 < 0) ? movetic : movetic - 1;

	                if(movetic % 100 == 0) {//arrive floor
	                    if(DEBUG == true)
	                        System.out.println("floorarrive");
	                    int num = movetic /100;
	                    //destination of elev or there is passenger in arrive floor
	                    this.elevator.nowFloor = num + 1;
	                    if(this.elevator.btnstate[num] == true || this.floors.get(num).buttonState.isContain(this.elevator.stateUpDown)) {
	                        if(DEBUG == true)
	                            System.out.println("stopping");
	                        this.openREQ = true;
	                        this.elevator.stateMove = StateMove.STOP;
	                        this.elevator.btnstate[num] = false;
	                        if(num == 6)
	                            this.elevator.stateUpDown = StateUpDown.DOWN;
	                        else if(num == 0)
	                            this.elevator.stateUpDown = StateUpDown.UP;
	                    }
	                }

	            }
	            n = getSwitch(dev_2);
	            btn = getIntrBtn(dev_1);
	            if(DEBUG == true && (n != -1 || btn != -1) )
				System.out.println(String.format("switch : %d  btn : %d\n",n,btn));
			
	            if(n != -1) {
	                if(0<= n && n <=6)
	                    this.elevator.btnstate[n] = true;
	                else if(n == 7 && this.elevator.stateMove == StateMove.STOP)//open
	                    this.openREQ = true;
	                
	                else if(n == 8 && this.elevator.stateMove == StateMove.STOP)//close
	                    stoptic = (stoptic < 70 )? 70 : stoptic;

	            } 
	            if(btn != -1){
	                if(btn == IntrBtn.VOLUP.getValue()){
	                    int floorNum = rnd.nextInt(6);
	                    this.floors.get(floorNum).addPerson(StateUpDown.UP);
	                }
	                else if(btn == IntrBtn.VOLDOWN.getValue()){
	                    int floorNum = rnd.nextInt(6)+1;
	                    this.floors.get(floorNum).addPerson(StateUpDown.DOWN);
	                }
	                else if(btn == IntrBtn.BACK.getValue()){
	                    this.elevator.evitPerson();
	                }
	            }

	            //callSyscall data;


	            if(this.elevator.stateMove == StateMove.MOVE)
	                this.calculTarget();
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
				*/
	            try { Thread.sleep(100); } catch (InterruptedException e) { e.printStackTrace(); }

	        }
	        //closeDevice(dev_1,dev_2);
	    }
	    public void stop(){
	        this.running = false;
	    }
	}
}


