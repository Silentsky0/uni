package migration.agents;

import jade.content.ContentManager;
import jade.content.lang.sl.SLCodec;
import jade.core.Agent;
import jade.domain.mobility.MobilityOntology;

public class MigratingAgent extends Agent {
    @Override
    protected void setup() {
        super.setup();
        ContentManager cm = getContentManager();
        cm.registerLanguage(new SLCodec());
        cm.registerOntology(MobilityOntology.getInstance());
        //register languages
        //register ontologies
        //add behaviours
    }
    @Override
    protected void afterMove() {
        super.afterMove();
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
