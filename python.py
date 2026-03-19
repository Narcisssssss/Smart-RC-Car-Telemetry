import socket
import json
import pygame


UDP_IP = "0.0.0.0"          
UDP_PORT = 5005             
ESP_IP = "192.168.1.135"    

# Inițializare Socket UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
sock.setblocking(False)     # Nu blochează programul dacă nu vin date

# Inițializare Interfață Grafică (Pygame)
pygame.init()
pygame.display.set_caption("Control Panel Robot - Telemetrie")
screen = pygame.display.set_mode((500, 400))
font = pygame.font.SysFont("Consolas", 24)
clock = pygame.time.Clock()

def send_command(cmd):
    """Trimite o singură literă către ESP8266"""
    try:
        sock.sendto(cmd.encode(), (ESP_IP, UDP_PORT))
    except Exception as e:
        print(f"Eroare trimitere: {e}")

running = True
# Valori implicite pentru telemetrie
telemetrie = {"dist": 0.0, "L": 1, "R": 1}

print(f"Control Panel pornit! Trimit comenzi către {ESP_IP}")
print("Taste: W (Fata), S (Spate), A (Stanga), D (Dreapta), SPACE (Stop)")

while running:
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
        # Când APĂS tasta (Mașina pornește)
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w: send_command('F')
            elif event.key == pygame.K_s: send_command('B')
            elif event.key == pygame.K_a: send_command('L')
            elif event.key == pygame.K_d: send_command('R')
            elif event.key == pygame.K_space: send_command('S')

        # Când ELIBEREZ tasta (Mașina se oprește)
        if event.type == pygame.KEYUP:
            if event.key in [pygame.K_w, pygame.K_s, pygame.K_a, pygame.K_d]:
                send_command('S') 

    
    try:
        data, addr = sock.recvfrom(1024)
        telemetrie = json.loads(data.decode())
    except:
        pass

   
    screen.fill((20, 20, 20))
    
    
    dist_color = (0, 255, 0) if telemetrie["dist"] > 20 else (255, 50, 50)
    dist_text = font.render(f"Distanta: {telemetrie['dist']:.2f} cm", True, dist_color)
    screen.blit(dist_text, (50, 100))
    
    ir_l_text = "Stanga: OBSTACOL" if telemetrie["L"] == 0 else "Stanga: LIBER"
    ir_r_text = "Dreapta: OBSTACOL" if telemetrie["R"] == 0 else "Dreapta: LIBER"
    screen.blit(font.render(ir_l_text, True, (200, 200, 200)), (50, 160))
    screen.blit(font.render(ir_r_text, True, (200, 200, 200)), (50, 200))

    pygame.display.flip()
    clock.tick(30)

pygame.quit()