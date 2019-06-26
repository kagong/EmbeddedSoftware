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
		return value;
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
	
}
enum IntrBtn{
        VOLUP(0),VOLDOWN(1),BACK(2);
	private final int value;
	private IntrBtn(int value){
		this.value = value;
	}
	public int getValue(){
		return value;
	}	
	
}
public class EVSystem implements Runnable{


    native int getSwitch();
    native int getIntrBtn();
    native void setDot(int flag);
    native void setLed(int flag);
    native int callSyscall();
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
        public Floor(int floorNum){
            this.people = new ArrayList<Person>();
            this.floorNum = floorNum;
            this.buttonState = StateUpDown.NONE;
        }
        public void addPerson(StateUpDown state){
            if(state == StateUpDown.UP||state == StateUpDown.DOWN){
                Person person = new Person(state);
                people.add(person);
                if (state == StateUpDown.UP && buttonState == StateUpDown.NONE)
                    this.buttonState = StateUpDown.UP;
                else if (state == StateUpDown.UP && buttonState == StateUpDown.DOWN)
                    this.buttonState = StateUpDown.UPDOWN;
                else if (state == StateUpDown.DOWN && buttonState == StateUpDown.NONE)
                    this.buttonState = StateUpDown.DOWN;
                else if (state == StateUpDown.DOWN && buttonState == StateUpDown.UP)
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
            stateMove = StateMove.STOP;
            stateUpDown = StateUpDown.NONE;
        }
        public void evitPerson(){
            if(stateMove == StateMove.STOP){
                this.personNum-=1;
            }
        }
    }
    List<Floor> floors;
    Elevator elevator;
    boolean running;
    int tic;
    //constructor for elevator system class 
    public EVSystem(){
	floors = new ArrayList<Floor>();
        for(int i = 0 ; i  < 7; i++){
            Floor floor = new Floor(i+1);
            this.floors.add(floor);
        }
        this.elevator = new Elevator();
        this.tic = 0;
    }
    public Floor getFloor(int i){//0부터 시작
        return floors.get(i);
    }
    public void run(){
        Random rnd = new Random();
        this.tic += 1;
        this.running = true;
        while(running){
            int n = getSwitch();
            int btn = getIntrBtn();
            elevator.btnstate[n] = true;
            if(btn == IntrBtn.VOLUP.getValue()){
                int floorNum = rnd.nextInt(6);
                floors.get(floorNum).addPerson(StateUpDown.UP);
            }
            else if(btn == IntrBtn.VOLDOWN.getValue()){
                int floorNum = rnd.nextInt(6)+1;
                floors.get(floorNum).addPerson(StateUpDown.DOWN);
            }
            else if(btn == IntrBtn.BACK.getValue()){
                elevator.evitPerson();
            }
            
            StateUpDown UpDown = StateUpDown.values()[callSyscall()];
            
            if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.NONE)
                setDot(0);
            if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.UP)
                setDot(1);
            if(UpDown !=elevator.stateUpDown && UpDown == StateUpDown.DOWN)
                setDot(2);
        }
    }
    public void stop(){
        this.running = false;
    }
}
