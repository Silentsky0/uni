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
        if (!myAgent.getLocationList().isEmpty()) {
            Location location = myAgent.getLocationList().get(0);
            myAgent.getLocationList().remove(location);
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            myAgent.getLocationList().add(location);
            myAgent.doMove(location);
        }

    }

    @Override
    public boolean done() {
        return myAgent.getLocationList().isEmpty();
    }

}