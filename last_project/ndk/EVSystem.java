public class EVSystem implements Runnable{
    native int getSwitch();
    native int getVolUp();
    native int getVolDown();
    native int getBack();
    native void callSyscall();
    public enum StateUpDown{
        NONE,UP,DOWN,UPDOWN
    }
    public enum StateMove{
        MOVE,STOP
    }
    static {
        System.loadLibrary("EVSystem");
    };    
    public class Floor(){
        private class Person(){
            StateUpDown state;
            private Person(State state){
                this.state = flag;
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
    }
    List<Floor> floors;
    Elevator elevator;

    //constructor for elevator system class 
    public EVSystem(){
        for(int i = 0 ; i  < 7; i++){
            Floor floor = new Floor(i+1);
            floors.add(floor);
        }
        elevator = new Elevator();
    }
    public void run(){
    }
    public void stop(){
    }
}
