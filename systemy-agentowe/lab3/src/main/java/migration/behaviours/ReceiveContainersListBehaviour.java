package migration.behaviours;

import jade.content.ContentElement;
import jade.content.lang.Codec;
import jade.content.lang.sl.SLCodec;
import jade.content.onto.OntologyException;
import jade.content.onto.basic.Result;
import jade.core.Location;
import jade.core.behaviours.Behaviour;
import jade.domain.FIPANames;
import jade.domain.mobility.MobilityOntology;
import jade.lang.acl.ACLMessage;
import jade.lang.acl.MessageTemplate;
import lombok.extern.java.Log;
import migration.agents.MigratingAgent;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

@Log
public class ReceiveContainersListBehaviour extends Behaviour {

    private boolean done = false;

    protected final MigratingAgent myAgent;

    protected final String conversationId;

    protected MessageTemplate mt;

    public ReceiveContainersListBehaviour(MigratingAgent agent, String conversationId) {
        super(agent);
        myAgent = agent;
        this.conversationId = conversationId;
    }

    @Override
    public void onStart() {
        super.onStart();
        mt = MessageTemplate.MatchConversationId(conversationId);
    }

    @Override
    public void action() {
        myAgent.getContentManager().registerLanguage(new SLCodec(), FIPANames.ContentLanguage.FIPA_SL);
        myAgent.getContentManager().registerOntology(MobilityOntology.getInstance());

        ACLMessage msg = myAgent.receive(mt);
        if (msg != null) {
            done = true;
            try {
                ContentElement ce = myAgent.getContentManager().extractContent(msg);
                jade.util.leap.List items = ((Result) ce).getItems();
                List<Location> locations = new ArrayList<>();
                items.iterator().forEachRemaining(i -> {
                    locations.add((Location) i);
                });
                locations.remove(myAgent.here());
                locations.add(myAgent.here());
                myAgent.setLocationList(locations);
                myAgent.addBehaviour(new MigratingBehaviour(myAgent));
            } catch (Codec.CodecException | OntologyException ex) {
                log.log(Level.WARNING, null, ex);
            }
        }
    }

    @Override
    public boolean done() {
        return done;
    }

}