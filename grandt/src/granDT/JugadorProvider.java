package granDT;


public class JugadorProvider {

	
	private static JugadorProvider instance;
	
	static boolean n=false; 
	
	public JugadorProvider getInstance(){
		if(instance == null) return new JugadorProvider();
		else return instance;
	}
	
	private JugadorProvider(){

	}
	
	public static Jugador getJugador(){
		if(n) return new Jugador("pepe", "boca", 1000L, "delantero",5);
		else return new Jugador("lalo","lanus",1222L,"defensor",7);

	}
	
}
