import java.util.List;
import java.util.Scanner;;
import java.util.ArrayList;
import java.util.Random;
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


    //native int getSwitch(int dev_id);
    //native int getIntrBtn(int dev_id);
    //native void setDot(int dev_id,int flag);
    //native void setLed(int dev_id,int data);
    //native void setBuzzer(int dev_id);

    //native int callSyscall(int data1,int data2,int[] data3,int[] data4);
    //native int openDevice1();
    //native int openDevice2();
    //native void closeDevice(int dev_id);

    //static {
    //  System.loadLibrary("EVSystem");
    //};    
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

        public Elevator(){
            this.personNum = 0;
            this.nowFloor = 1;
            for(int i=0;i<7;i++)
                btnstate[i] = false;
            this.stateMove = StateMove.STOP;
            this.stateUpDown = StateUpDown.NONE;
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
    boolean running;

    //constructor for elevator system class 
    public EVSystem(){
        floors = new ArrayList<Floor>();
        for(int i = 0 ; i  < 7; i++){
            Floor floor = new Floor(i+1);
            this.floors.add(floor);
        }
        this.elevator = new Elevator();
    }
    public Floor getFloor(int i){//0�겫占쏙옙苑� 占쎈뻻占쎌삂
        return this.floors.get(i);
    }
    public void run(){
        int stoptic=0,movetic=0,data;
        boolean isOpen =false,openningREQ = true;
        Random rnd = new Random();
        this.running = true;
        //openDevice();
        //openDevice();
        while(running){
            int n = -1;
            int btn = -1;
            // stop action
            if(this.elevator.stateMove == StateMove.STOP) {
                boolean isIdle = true;

                System.out.println("stop");
                for(int i = 0 ; isOpen == false && i < 7 ; i++) {//idle
                    if(this.elevator.btnstate[i] == true || this.floors.get(i).buttonState != StateUpDown.NONE){
                        if(i == this.elevator.nowFloor - 1){

                            openningREQ = true;
                        }
                        else{
                            isIdle = false;
                        }
                    }
                }
                if(isIdle)
                    System.out.println("idle");
                if(openningREQ == true){
                    System.out.println("open");
                    openningREQ = false;
                    stoptic = 0;
                    if(isOpen == false){
                        isOpen = true;
                        //openning voice
                    }
                }
                stoptic = (stoptic + 1 > 80)? stoptic : stoptic + 1;
                if(isOpen) {//open door or not idle
                    if(stoptic >= 80) {//8sec
                        System.out.println("closing");
                        isOpen = false;
                        isIdle = true;
                        for(int i = 0 ; isOpen == false && i < 7 ; i++) {//idle
                            if(this.elevator.btnstate[i] == true || this.floors.get(i).buttonState != StateUpDown.NONE){
                                if(i == this.elevator.nowFloor - 1){
                                }
                                else{
                                    isIdle = false;
                                }
                            }
                        }


                        if(isIdle == false){
                            System.out.println("conver to move");
                            this.elevator.stateMove = StateMove.MOVE;
                        }

                        //closing voice
                    }
                    else{
                        Floor temp = this.floors.get(this.elevator.nowFloor-1);
                        boolean flag2 =false;
                        for(int i=0 ; i < temp.people.size();++i) {
                            if( temp.people.get(i).state == this.elevator.stateUpDown) {
                                if(this.elevator.addPerson()) {
                                    flag2 = true;
                                    temp.people.remove(i); 
                                }
                                else{
                                    flag2 = false;	
                                    // setBuzzer();//1sec ring
                                }
                            }
                        }
                        if(flag2)
                            temp.buttonState = StateUpDown.getState(temp.buttonState.getValue() - this.elevator.stateUpDown.getValue());

                    }

                }
                else
                    this.elevator.stateUpDown = StateUpDown.NONE;


            }
            else {//move 10sec
                System.out.println("move");
                if(this.elevator.stateUpDown == StateUpDown.UP) 
                    movetic = (movetic + 1 > 600) ? movetic : movetic + 1;
                else if(this.elevator.stateUpDown == StateUpDown.DOWN) 
                    movetic = (movetic - 1 < 0) ? movetic : movetic - 1;

                if(movetic % 100 == 0) {//arrive floor
                    System.out.println("floor");
                    int num = movetic /100;
                    //destination of elev or there is passenger in arrive floor
                    this.elevator.nowFloor = num + 1;
                    if(this.elevator.btnstate[num] == true || this.floors.get(num).buttonState.isContain(this.elevator.stateUpDown)) {
                        System.out.println("stop");
                        openningREQ = true;
                        this.elevator.stateMove = StateMove.STOP;
                        this.elevator.btnstate[num] = false;
                        if(num == 6)
                            this.elevator.stateUpDown = StateUpDown.DOWN;
                        else if(num == 0)
                            this.elevator.stateUpDown = StateUpDown.UP;
                    }
                }

            }
            //n = getSwitch();
            //btn = getIntrBtn();
            if(n != -1) {
                if(0<= n && n <=6)
                    this.elevator.btnstate[n] = true;
                else if(n == 7 && this.elevator.stateMove == StateMove.STOP)//open
                    openningREQ = true;
                else if(n == 8 && this.elevator.stateMove == StateMove.STOP)//close
                    stoptic = (stoptic < 70 )? 70 : stoptic;

            }
            if(isOpen == true){

                System.out.print("input: ");
                Scanner scan = new Scanner(System.in);
                btn = scan.nextInt();
                if(stoptic < 78)
                    stoptic = 78;
            }
            if(btn != -1){
                if(btn == IntrBtn.VOLUP.getValue()){
                    int floorNum = rnd.nextInt(6);
                    this.floors.get(floorNum).addPerson(StateUpDown.UP);

                    System.out.println(Integer.toString(floorNum));
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

            StateUpDown UpDown = elevator.stateUpDown;

            int data1 = this.elevator.nowFloor;//start 1
            int data2 = this.elevator.stateUpDown.getValue();
            int data3[] = new int[7];
            int data4[] = new int[7];
            for(int i= 0 ; i < 7 ;i++) {
                data3[i] = this.floors.get(i).buttonState.getValue();
                data4[i] = (this.elevator.btnstate[i] == true)? 1: 0;
            }
            elevator.stateUpDown = StateUpDown.UP;
            //if(this.elevator.stateMove == StateMove.MOVE)
            //	UpDown = StateUpDown.values()[callSyscall(data1,data2,data3,data4)];

            //if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.NONE)
            //  this.setDot(0);
            //if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.UP)
            //  this.setDot(1);
            //if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.DOWN)
            //  this.setDot(2);
            data = 0;
            for(int i = 0 ; i < 7 ; i++) 
                data += ((this.elevator.btnstate[i] == true)? 1: 0) << i;
            //this.setLed(data);

            try { Thread.sleep(100); } catch (InterruptedException e) { e.printStackTrace(); }

        }
        //closeDevice();
        //closeDevice();
    }
    public void stop(){
        this.running = false;
    }
    public static void main(String[] args){
        EVSystem temp = new EVSystem();
        System.out.println("fuck\n");
        temp.run();

    }
}
