root -l -b <<-EOF
.x digi.C
.q
EOF

root -l -b <<-EOF
.x ADC_analysis.C
.q
EOF

root -l -b <<-EOF
.x Particle_tracking.C
.q
EOF
