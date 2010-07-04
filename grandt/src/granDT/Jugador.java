package granDT;

public class Jugador {

	private String nombre;
	private String equipo;
	private String puesto;
	private Long costo;
	private int  puntajes;
	
	
	public Jugador(String nombre,
					String equipo,
					Long costo,
					String puesto,
					int  puntajes ){
		this.nombre=nombre;
		this.equipo=equipo;
		this.costo=costo;
		this.puesto=puesto;
		this.puntajes=puntajes;
		
	}


	public String getNombre() {
		return nombre;
	}


	public String getEquipo() {
		return equipo;
	}


	public String getPuesto() {
		return puesto;
	}


	public Long getCosto() {
		return costo;
	}


	public int getPuntajes() {
		return puntajes;
	}
/*
	public int getPuntajeFecha(int nFecha){
		return puntajes[nFecha];
	}
*/

}
