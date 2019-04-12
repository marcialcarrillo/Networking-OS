Buzon:
	   Types
			1 ack constratista al lector. Informar que ya termino 
			2 ack emisor al lector. Informar que ya recibio el nombre del archivo
			4 Comienza la comunicacion con el Emisor y/o Receptor, contiene el nombre, id y total de paquetes
			5 ack Lector al emisor. Informa que ya no hay mas imagenes
			6 ack Hilos Emisor con el Padre Emisor. Informan que ya terminaron de escribir la imagen
			Id ack Emisor al contratista. Informa con el id de la imagen que ya termino de escribirla
			7 ack receptor al emisor. Informa que termino de escribir el archivo
			8 ack emisor al receptor. Informa que ya no hay mas imagenes
			2019-2xxx // Cada contratista tiene un identificador unico(inicia en 2019)
	  
			
Lector:
		Ubica las imagenes
		Crea un contratista(proceso) por imagen(A lo mas 2) con un identificador (inicia en 2019)
			Espera un ack(1) del contratista para crear otro
		Cuando terminan todos los contratistas manda un ack(5)
		
		
Contratista:
		Envia el nombre del archivo y el identificador al emisor
		Espera un ack(2) del emisor
		Envia paquetes de 128bytes al emisor
		Se envia un ultimo paquete informando que se termino la imagen 	
		Espera un ack(3)
		Envia un ack(1) al lector informando que ya termino

Emisor:
	Espera sin bloqueo un ack(5)
	Recibe un paquete tipo 4
	Envia paquete tipo 4 al receptor
	//Espera un ack(6)
	Envia un ack(2)
	Espera sin bloqueo un ack(7)
		Envia un ack(3)
	
	
Receptor:
	Espera sin bloqueo un ack(5)
	Recibe un paquete tipo 4
	//Envia un ack(6)
	Escribe la imagen
	Envia un ack(7)

