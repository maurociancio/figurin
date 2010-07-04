package granDT;

import org.jgap.InvalidConfigurationException;
import org.jgap.gp.function.RandomGenerator;
import org.jgap.gp.impl.GPConfiguration;

public class JugadorRandomGenerator extends RandomGenerator {

	public JugadorRandomGenerator(GPConfiguration conf, Class clase)
			throws InvalidConfigurationException {
		super(conf, clase);
		// TODO Auto-generated constructor stub
	}
	
	public Jugador getRandomJugador(){
		return JugadorProvider.getJugador();
	}

	private static final long serialVersionUID = 1L;

}
