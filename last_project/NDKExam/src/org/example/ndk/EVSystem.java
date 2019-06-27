package org.example.ndk;

import java.util.List;
import java.util.ArrayList;
import java.util.Random;

import android.util.Log;


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
    
    public void run(){
        int stoptic=0,movetic=0,data;
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
            /*
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
            if(DEBUG == true&& (n != -1 || btn != -1) )
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
            setLed(dev_1,10);
            setDot(dev_1,2);
            try { Thread.sleep(100); } catch (InterruptedException e) { e.printStackTrace(); }

        }
        closeDevice(dev_1,dev_2);
    }
    public void stop(){
        this.running = false;
    }

}
