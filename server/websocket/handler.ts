import { Server as SocketIOServer } from 'socket.io';

export function initializeWebSocket(io: SocketIOServer) {
  io.on('connection', (socket) => {
    console.log(`Client connected: ${socket.id}`);

    // Join room for specific user
    socket.on('join', (data) => {
      const { userId, room } = data;
      socket.join(`user-${userId}`);
      if (room) {
        socket.join(room);
      }
      console.log(`User ${userId} joined rooms`);
    });

    // Subscribe to real-time feeds
    socket.on('subscribe', (feed) => {
      socket.join(feed);
      console.log(`Socket ${socket.id} subscribed to ${feed}`);
    });

    // Unsubscribe from feeds
    socket.on('unsubscribe', (feed) => {
      socket.leave(feed);
      console.log(`Socket ${socket.id} unsubscribed from ${feed}`);
    });

    // Handle disconnection
    socket.on('disconnect', () => {
      console.log(`Client disconnected: ${socket.id}`);
    });

    // Ping/Pong for connection health
    socket.on('ping', () => {
      socket.emit('pong', { timestamp: Date.now() });
    });
  });

  return io;
}

// Helper function to broadcast to specific rooms
export function broadcastToRoom(io: SocketIOServer, room: string, event: string, data: any) {
  io.to(room).emit(event, data);
}

// Helper function to broadcast to specific user
export function broadcastToUser(io: SocketIOServer, userId: number, event: string, data: any) {
  io.to(`user-${userId}`).emit(event, data);
}

export default { initializeWebSocket, broadcastToRoom, broadcastToUser };
