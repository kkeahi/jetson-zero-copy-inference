# Jetson TX2 setup script

set -e	# exit on error

echo "Starting NVIDIA Jetson TX2 setup..."

# Step 1: Update system
echo "Step 1: Updating system packages..."
apt update && apt upgrade -y
