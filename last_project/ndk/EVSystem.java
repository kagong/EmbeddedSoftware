
import java.util.Random
public enum StateUpDown{
    NONE,UP,DOWN,UPDOWN
}
public enum StateMove{
        MOVE,STOP
}
public enum IntrBtn{
        VOLUP,VOLDOWN,BACK
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
    public class Floor(){
        public class Person(){
            private StateUpDown state;
            public Person(State state){
                this.state = flag;
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
            this.people = new List<Person>();
            this.floorNum = floorNum;
            this.buttonState = NONE;
        }
        public addPerson(StateUpDown state){
            if(state == UP||state == DOWN){
                Person person = new Person(state);
                people.add(person);
                if (people.state == UP && buttonState == NONE)
                    buttonState = UP;
                else if (people.state == UP && buttonState == DOWN)
                    buttonState = UPDOWN;
                else if (people.state == DOWN && buttonState == NONE)
                    buttonState = DOWN;
                else if (people.state == DOWN && buttonState == UP)
                    buttonState = UPDOWN;
            }
            else{
                //error
            }
        }
        public List<Person> getPeople(){
            return this.people;
        }
    }
    public class Elevator(){
        int personNum;
        int nowFloor;
        boolean btnstate[7];
        StateMove stateMove;
        StateUpDown stateUpDown;
        public Elevator(){
            this.personNum = 0;
            thos.nowFloor = 1;
            for(int i=0;i<7;i++)
                btnstate[i] = false;
            stateMove = STOP;
            stateUpDown = NONE;
        }
        public void evitPerson(){
            if(StateMove == STOP){
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
        for(int i = 0 ; i  < 7; i++){
            Floor floor = new Floor(i+1);
            this.floors.add(floor);
        }
        this.elevator = new Elevator();
        this.tic = 0;
    }
    public int getFloor(int i){//0부터 시작
        return floors.get(i);
    }
    public void run(){
        Random rnd = new Random();
        this.tic += 1;
        this.running = true;
        while(running){
            int n = getSwitch();
            IntrBtn btn = getIntrBtn();
            elevator.btnstate[n] = true;
            if(btn == VOLUP){
                int floorNum = rnd.nextInt(6);
                floors.get(floorNum).addPerson(UP);
            }
            else if(btn == VOLDOWN){
                int floorNum = rnd.nextInt(6)+1;
                floors.get(floorNum).addPerson(DOWN);
            }
            else if(btn == BACK){
                elevator.evitPerson();
            }
            
            StateUpDown UpDown = callSyscall();
            
            if(Updown !=elevator.stateUpDown && UpDown == NONE)
                setDot(0);
            if(Updown !=elevator.stateUpDown && UpDown == UP)
                setDot(1);
            if(Updown !=elevator.stateUpDown && UpDown == DOWN)
                setDot(2);
        }
    }
    public void stop(){
        this.running = false;
    }
}
