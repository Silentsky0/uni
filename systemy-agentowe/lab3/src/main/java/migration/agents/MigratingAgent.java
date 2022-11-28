package migration.agents;

import jade.content.ContentManager;
import jade.content.lang.sl.SLCodec;
import jade.core.Agent;
import jade.core.Location;
import jade.domain.FIPANames;
import jade.domain.mobility.MobilityOntology;
import lombok.Getter;
import lombok.Setter;
import migration.behaviours.RequestContainersListBehaviour;

import java.util.List;

public class MigratingAgent extends Agent {
    @Getter
    @Setter
    private List<Location> locationList;
    private Location homeLocation;
    @Override
    protected void setup() {
        super.setup();
        ContentManager cm = getContentManager();
        cm.registerLanguage(new SLCodec(), FIPANames.ContentLanguage.FIPA_SL);
        cm.registerOntology(MobilityOntology.getInstance());

        System.out.println(this.getName().split("@")[0] + ": w lokacji " + this.here());
        this.homeLocation = this.here();
        //register languages
        //register ontologies
        //add behaviours
        this.addBehaviour(new RequestContainersListBehaviour(this));
    }
    @Override
    protected void afterMove() {
        super.afterMove();
        System.out.println(this.getName().split("@")[0] + ": w lokacji " + this.here());
        if (this.here().equals(homeLocation)) {
            System.out.println(this.getName().split("@")[0] + ": Requestuje nowe lokacje");
            this.getLocationList().clear();
            System.out.println("locations size = " + this.locationList.size());
            this.addBehaviour(new RequestContainersListBehaviour(this));
        }

        //restore state
        //resume threads
    }
    @Override
    protected void beforeMove() {
        //stop threads
        //save state
        super.beforeMove();
    }
}