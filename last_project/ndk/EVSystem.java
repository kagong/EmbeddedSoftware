import java.util.List;
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


    native int getSwitch();
    native int getIntrBtn();
    native void setDot(int flag);
    native void setLed(int data);
    native void setBuzzer();

    native int callSyscall(int data1,int data2,int[] data3,int[] data4);
    native void openDevice();
    native void closeDevice();

    static {
        System.loadLibrary("EVSystem");
    };    
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
        Random rnd = new Random();
        this.running = true;
        openDevice();
        while(running){
            int n;
            int btn;
            if(this.elevator.stateMove == StateMove.STOP) {
            	
            	boolean flag = false;
                stoptic = (stoptic + 1 > 80)? stoptic : stoptic + 1;
                for(int i = 0 ; i < 7 ; i++) {//idle
                	if(this.elevator.btnstate[i] == true || this.floors.get(i).buttonState != StateUpDown.NONE)
                		flag = true;
                }
                if(flag) {
            		if(stoptic >= 80) {//8sec
            			stoptic = 0;
            			this.elevator.stateMove = StateMove.MOVE;
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
							        setBuzzer();//1sec ring
						        }
            			    }
            		    }
            			if(flag2)
                            temp.buttonState = StateUpDown.getState(temp.buttonsState.getValue() - this.elevator.stateUpDown);
				
            		}
			
                }
                else
                    this.elevator.stateUpDown = StateUpDown.NONE;
                
            	
            }
            else {//move 10sec
        		if(this.elevator.stateUpDown == StateUpDown.UP) 
                	movetic = (movetic + 1 > 600) ? movetic : movetic + 1;
        		else if(this.elevator.stateUpDown == StateUpDown.DOWN) 
                	movetic = (movetic - 1 < 0) ? movetic : movetic - 1;

            	if(movetic % 100 == 0) {//arrive floor
            		int num = movetic /100;
            		//destination of elev or there is passenger in arrive floor
            		if(this.elevator.btnstate[num] == true || this.floors.get(num).buttonState.isContain(this.elevator.stateUpDown)) {
            			stoptic = 0;
            			this.elevator.stateMove = StateMove.STOP;
            			this.elevator.btnstate[num] = false;
            			this.elevator.nowFloor = num + 1;
            			if(num == 6)
            				this.elevator.stateUpDown = StateUpDown.DOWN;
            			else if(num == 0)
            				this.elevator.stateUpDown = StateUpDown.UP;
            		}
            	}
            	
            }
            n = getSwitch();
            btn = getIntrBtn();
            if(n != -1) {
            	if(0<= n && n <=6)
            		this.elevator.btnstate[n] = true;
            	else if(n == 7 && this.elevator.stateMove == StateMove.STOP)//open
            		stoptic = 0;
            	else if(n == 8 && this.elevator.stateMove == StateMove.STOP)//close
            		stoptic = 70;
            		
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
            
            StateUpDown UpDown = elevator.stateUpDown;

            int data1 = this.elevator.nowFloor;//start 1
            int data2 = this.elevator.stateUpDown.getValue();
            int data3[] = new int[7];
            int data4[] = new int[7];
            for(int i= 0 ; i < 7 ;i++) {
            	data3[i] = this.floors.get(i).buttonState.getValue();
            	data4[i] = (this.elevator.btnstate[i] == true)? 1: 0;
            }

            if(this.elevator.stateMove == StateMove.MOVE)
            	UpDown = StateUpDown.values()[callSyscall(data1,data2,data3,data4)];
            
            if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.NONE)
                this.setDot(0);
            if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.UP)
                this.setDot(1);
            if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.DOWN)
                this.setDot(2);
            data = 0;
            for(int i = 0 ; i < 7 ; i++) 
            	data += ((this.elevator.btnstate[i] == true)? 1: 0) << i;
            this.setLed(data);

		try { Thread.sleep(100); } catch (InterruptedException e) { e.printStackTrace(); }
//0.1 sec
        }
        closeDevice();
    }
    public void stop(){
        this.running = false;
    }
    public static void main(String[] args){
        EVSystem temp = new EVSystem();
        temp.run();
        for(int i = 0; i < 7 ; i++){
            System.out.println("%d floor %d person",i,);
        }

    }
}
