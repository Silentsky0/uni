package migration.behaviours;

import jade.core.Location;
import jade.core.behaviours.Behaviour;
import migration.agents.MigratingAgent;

public class MigratingBehaviour extends Behaviour {

    protected final MigratingAgent myAgent;

    public MigratingBehaviour(MigratingAgent agent) {
        super(agent);
        myAgent = agent;
    }

    @Override
    public void action() {
        Location location = myAgent.getLocationList().get(0);
        myAgent.getLocationList().remove(location);
        //myAgent.getLocations().add(location); //odkomentowac dla zad3
        myAgent.doMove(location);
    }

    @Override
    public boolean done() {
        return myAgent.getLocationList().isEmpty();
    }

}